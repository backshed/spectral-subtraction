#include "subtraction_manager.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <chrono>
#include "math_util.h"


//Linux only, for audio out (fifo)
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

SubtractionManager::SubtractionManager(int fft_Size, int sampling_Rate):
	_fftSize(fft_Size),
	_samplingRate(sampling_Rate),
	_data(nullptr),
	_origData(nullptr),
	_tabLength(0),
	_useOLA(false),
	_bypass(false)
{
	onFFTSizeUpdate();
}



void SubtractionManager::execute()
{
	// Some configuration and cleaning according to the parameters used
	if (bypass()) return;
	if (dataSource() == DataSource::File)
	{
		initDataArray();
	}
	// For Julius, call onDataUpdate() on every file change, and only once if it is mic input.

	// Execution of the algortihm
	for (auto iter = 0U; iter < iterations(); ++iter)
	{
		for (auto sample_n = 0U; sample_n < getLength(); sample_n += getFrameIncrement())
		{
			copyInput(sample_n);
			forwardFFT();

			if(dataSource() == DataSource::File && sample_n == 0)
				onDataUpdate();

			// Noise estimation
			(*getEstimationImplementation())(spectrum());

			// Spectral subtraction
			(*getSubtractionImplementation())(spectrum(), getEstimationImplementation()->noisePower());

			backwardFFT();
			copyOutput(sample_n);
		}
	}
}



void SubtractionManager::onFFTSizeUpdate()
{
	if(_bypass) return;
	_spectrumSize = _fftSize / 2 + 1;
	_ola_frame_increment = _fftSize / 2;
	_std_frame_increment = _fftSize;

	in = fftw_alloc_real(_fftSize);
	out = fftw_alloc_real(_fftSize);

	_spectrum = fftw_alloc_complex(_spectrumSize);

	// Initialize the fftw plans
	plan_fw = fftw_plan_dft_r2c_1d(_fftSize, in, _spectrum, FFTW_ESTIMATE);
	plan_bw = fftw_plan_dft_c2r_1d(_fftSize, _spectrum, out, FFTW_ESTIMATE);

	if(estimation) estimation->onFFTSizeUpdate();
	if(subtraction) subtraction->onFFTSizeUpdate();
}


void SubtractionManager::copyInput(unsigned int pos)
{
	if(_useOLA)
		copyInputOLA(pos);
	else
		copyInputSimple(pos);
}

void SubtractionManager::copyOutput(unsigned int pos)
{
	if(_useOLA)
		copyOutputOLA(pos);
	else
		copyOutputSimple(pos);
}

void SubtractionManager::clean()
{
	fftw_free(in);
	fftw_free(out);

	fftw_free(_spectrum);
}

SubtractionManager::~SubtractionManager()
{
	clean();

	delete[] _data;
	delete[] _origData;

}

void SubtractionManager::initDataArray()
{
	std::copy_n(_origData, _tabLength, _data);
}

unsigned int SubtractionManager::iterations() const
{
	return _iterations;
}

void SubtractionManager::setIterations(int value)
{
	_iterations = std::max(value, 1);
}


double *SubtractionManager::getData()
{
	return _data;
}

void SubtractionManager::onDataUpdate()
{
	if(_bypass) return;
	estimation->onDataUpdate();
	subtraction->onDataUpdate();
}

double *SubtractionManager::getNoisyData()
{
	return _origData;
}

unsigned int SubtractionManager::getLength()
{
	return _tabLength;
}

unsigned int SubtractionManager::readFile(char *str)
{
	std::ifstream ifile(str, std::ios_base::ate | std::ios_base::binary);
	_tabLength = ifile.tellg() / (sizeof(short) / sizeof(char));
	ifile.clear();
	ifile.seekg(0, std::ios_base::beg);

	delete[] _origData;
	delete[] _data;
	_origData = new double[_tabLength];
	_data = new double[_tabLength];

	// We have to get signal between -1 and 1
	static const double normalizationFactor = pow(2.0, sizeof(short) * 8 - 1.0);

	unsigned int pos = 0;
	short sample;
	while (ifile.read((char *)&sample, sizeof(short)) && pos < _tabLength)
	{
		_origData[pos++] = sample / normalizationFactor;
	}

	ifile.close();
	_dataSource = DataSource::File;
	return _tabLength;
}

unsigned int SubtractionManager::readBuffer(short *buffer, int length)
{
	_tabLength = length;
	if(_bypass) return length;

	delete[] _origData;
	delete[] _data;
	_origData = new double[_tabLength];
	_data = new double[_tabLength];

	// Julius accepts only big-endian raw files but it seems internal buffers
	// are little-endian so no need to convert.
	// std::transform(buffer, buffer + tab_length, buffer,
	//                [] (short val) {return (val << 8) | ((val >> 8) & 0xFF)});

	std::transform(buffer, buffer + _tabLength, _origData, MathUtil::ShortToDouble);
	initDataArray();

	_dataSource = DataSource::Buffer;
	return _tabLength;
}

void SubtractionManager::writeBuffer(short *buffer)
{
	if(_bypass) return;
	std::transform(_data, _data + _tabLength, buffer, MathUtil::DoubleToShort);

	// Julius accepts only big-endian raw files but it seems internal buffers
	// are little-endian so no need to convert.
	// std::transform(buffer, buffer + tab_length, buffer,
	//                [] (short val) {return (val << 8) | ((val >> 8) & 0xFF)});
}

void SubtractionManager::copyInputSimple(unsigned int pos)
{
	// Data copying
	if (_fftSize <= _tabLength - pos)
	{
		std::copy_n(_data + pos, _fftSize, in);
	}
	else
	{
		std::copy_n(_data + pos, _tabLength - pos, in);
		std::fill_n(in + _tabLength - pos, _fftSize - (_tabLength - pos), 0);
	}
}

void SubtractionManager::copyOutputSimple(unsigned int pos)
{
	auto normalizeFFT = [&](double x) { return x / _fftSize; };
	if (_fftSize <= _tabLength - pos)
	{
		std::transform(out, out + _fftSize, _data + pos, normalizeFFT);
	}
	else //fileSize - pos < fftSize
	{
		std::transform(out, out + _tabLength - pos, _data + pos, normalizeFFT);
	}
}

void SubtractionManager::copyInputOLA(unsigned int pos)
{
	// Data copying
	if (_ola_frame_increment <= _tabLength - pos) // last case
	{
		std::copy_n(_data + pos, _ola_frame_increment, in);
		std::fill_n(in + _ola_frame_increment, _ola_frame_increment, 0);

		std::fill_n(_data + pos, _ola_frame_increment, 0);
	}
	else
	{
		std::copy_n(_data + pos, _tabLength - pos, in);
		std::fill_n(in + _tabLength - pos, _ola_frame_increment - (_tabLength - pos), 0);

		std::fill_n(_data + pos, _tabLength - pos, 0);
	}
}

void SubtractionManager::copyOutputOLA(unsigned int pos)
{
	// Lock here
	//ola_mutex.lock();
	for (auto j = 0U; (j < _fftSize) && (pos + j < _tabLength); ++j)
	{
		_data[pos + j] += out[j] / _fftSize;
	}
	// Unlock here
	//ola_mutex.unlock();
}
bool SubtractionManager::OLAenabled() const
{
	return _useOLA;
}

void SubtractionManager::enableOLA()
{
	_useOLA = true;
}

void SubtractionManager::disableOLA()
{
	_useOLA = false;
}

void SubtractionManager::setOLA(bool val)
{
	_useOLA = val;
}

void SubtractionManager::forwardFFT()
{
	fftw_execute(plan_fw);
}

void SubtractionManager::backwardFFT()
{
	fftw_execute(plan_bw);
}

fftw_complex *SubtractionManager::spectrum()
{
	return _spectrum;
}

SubtractionManager::DataSource SubtractionManager::dataSource() const
{
	return _dataSource;
}

std::shared_ptr<Estimation> SubtractionManager::getEstimationImplementation() const
{
	return estimation;
}

void SubtractionManager::setEstimationImplementation(std::shared_ptr<Estimation> value)
{
	estimation = std::move(value);
	estimation->onFFTSizeUpdate();
}

bool SubtractionManager::bypass()
{
	return _bypass;
}


unsigned int SubtractionManager::getFrameIncrement()
{
	return _useOLA? _ola_frame_increment : _std_frame_increment;
}

std::shared_ptr<Subtraction> SubtractionManager::getSubtractionImplementation() const
{
	return subtraction;
}

void SubtractionManager::setSubtractionImplementation(std::shared_ptr<Subtraction> value)
{
	subtraction = std::move(value);
	subtraction->onFFTSizeUpdate();
}

unsigned int SubtractionManager::getSamplingRate() const
{
	return _samplingRate;
}

void SubtractionManager::setSamplingRate(unsigned int value)
{
	_samplingRate = value;
	clean();
	onFFTSizeUpdate();
}

/*
 * File syntax :
 * alpha
 * beta
 * alphawt
 * betawt
 * iterations
 * noise algo (std / martin / wavelets)
 * algo (std / el / ga)
*/

void SubtractionManager::readParametersFromFile()
{
	_bypass = false;
	std::map<std::string, std::shared_ptr<Estimation>> noise_est
	{
		std::make_pair("std", std::shared_ptr<Estimation>(new SimpleEstimation(*this))),
		std::make_pair("martin", std::shared_ptr<Estimation>(new MartinEstimation(*this))),
		std::make_pair("wavelets", std::shared_ptr<Estimation>(new WaveletEstimation(*this)))
	};

	static const std::map<std::string, Subtraction::Algorithm> algo
	{
		std::make_pair("std", Subtraction::Algorithm::Standard),
		std::make_pair("el", Subtraction::Algorithm::EqualLoudness),
		std::make_pair("ga", Subtraction::Algorithm::GeometricApproach),
		std::make_pair("bypass", Subtraction::Algorithm::Bypass)
	};

	std::ifstream f("subtraction.conf");
	double alpha, beta, alphawt, betawt;
	int iterations;
	std::string noise_alg, alg;

	// Class members
	f >> alpha;
	f >> beta;
	f >> alphawt;
	f >> betawt;
	f >> iterations;

	// Local stuff
	f >> noise_alg;
	f >> alg;
	f.close();

	setIterations(iterations);

	if (noise_est.find(noise_alg) != noise_est.end())
		setEstimationImplementation(noise_est[noise_alg]);
	else
		std::cerr << "Invalid estimation algorithm";

	if (algo.find(alg) != algo.end())
	{
		switch(algo.at(alg))
		{
			case Subtraction::Algorithm::Standard:
			{
				// Check for memory leak.
				SimpleSpectralSubtraction* subtraction = new SimpleSpectralSubtraction(*this);
				subtraction->setAlpha(alpha);
				subtraction->setBeta(beta);
				setSubtractionImplementation(std::shared_ptr<Subtraction>(subtraction));
				break;
			}
			case Subtraction::Algorithm::EqualLoudness:
			{
				EqualLoudnessSpectralSubtraction* zubtraction = new EqualLoudnessSpectralSubtraction(*this);
				zubtraction->setAlpha(alpha);
				zubtraction->setBeta(beta);
				zubtraction->setAlphawt(alphawt);
				zubtraction->setBetawt(betawt);
				setSubtractionImplementation(std::shared_ptr<Subtraction>(zubtraction));
				break;
			}
			case Subtraction::Algorithm::GeometricApproach:
			{
				setSubtractionImplementation(std::shared_ptr<Subtraction>(new GeometricSpectralSubtraction(*this)));
				break;
			}
			default:
				_bypass = true;
		}
	}
	else
	{
		std::cerr << "Invalid subtraction algorithm";
	}
}

unsigned int SubtractionManager::FFTSize() const
{
	return _fftSize;
}

void SubtractionManager::setFftSize(unsigned int value)
{
	_fftSize = value;
	clean();
	onFFTSizeUpdate();
}

unsigned int SubtractionManager::spectrumSize() const
{
	return _spectrumSize;
}