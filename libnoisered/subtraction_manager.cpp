#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

#include "subtraction_manager.h"
#include "mathutils/math_util.h"
#include "fft/fftwmanager.h"


SubtractionManager::SubtractionManager(const unsigned int fft_Size, const unsigned int sampling_Rate):
	_samplingRate(sampling_Rate),
	_fft(new FFTWManager)
{
	_fft->updateSize(fft_Size);
	onFFTSizeUpdate();
}

SubtractionManager::SubtractionManager(const SubtractionManager &sm):
	_dataSource(sm._dataSource),
	_samplingRate(sm.getSamplingRate()),
	_fft(nullptr),
	_subtraction(nullptr),
	_estimation(nullptr), //TODO Clone method
	_tabLength(sm._tabLength),
	_data(new double[_tabLength]),
	_origData(new double[_tabLength]),
	_useOLA(sm._useOLA),
	_iterations(sm.iterations())

{
	_fft.reset(sm._fft->clone());
	_subtraction.reset(sm._subtraction->clone());
	_estimation.reset(sm._estimation->clone());

	_fft->updateSize(sm._fft->size());
	onFFTSizeUpdate();
	std::copy_n(sm._data, _tabLength, _data);
	std::copy_n(sm._origData, _tabLength, _origData);
}

const SubtractionManager &SubtractionManager::operator=(const SubtractionManager &sm)
{
	_dataSource = sm._dataSource;
	_samplingRate = sm.getSamplingRate();

	_tabLength = sm._tabLength;
	delete[] _data;
	delete[] _origData;
	_data = new double[_tabLength];
	_origData = new double[_tabLength];
	_useOLA = sm._useOLA;
	_iterations = sm.iterations();

	_fft.reset(sm._fft->clone());
	_subtraction.reset(sm._subtraction->clone());
	_estimation.reset(sm._estimation->clone());

	_fft->updateSize(sm._fft->size());
	onFFTSizeUpdate();
	std::copy_n(sm._data, _tabLength, _data);
	std::copy_n(sm._origData, _tabLength, _origData);

	return *this;
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
			_fft->forward();

			if(dataSource() == DataSource::File && sample_n == 0)
				onDataUpdate();

			// Noise estimation
			(*getEstimationImplementation())(_fft->spectrum());

			// Spectral subtraction
			(*getSubtractionImplementation())(_fft->spectrum(), getEstimationImplementation()->noisePower());

			_fft->backward();
			copyOutput(sample_n);
		}
	}
}



void SubtractionManager::onFFTSizeUpdate()
{
	if(_bypass) return;

	_ola_frame_increment = _fft->size() / 2;
	_std_frame_increment = _fft->size();

	if(_estimation) _estimation->onFFTSizeUpdate();
	if(_subtraction) _subtraction->onFFTSizeUpdate();
}


void SubtractionManager::copyInput(const unsigned int pos)
{
	if(_useOLA)
		copyInputOLA(pos);
	else
		copyInputSimple(pos);
}

void SubtractionManager::copyOutput(const unsigned int pos)
{
	if(_useOLA)
		copyOutputOLA(pos);
	else
		copyOutputSimple(pos);
}


SubtractionManager::~SubtractionManager()
{
	_subtraction.reset();
	_estimation.reset();

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

void SubtractionManager::setIterations(const unsigned int value)
{
	_iterations = std::max(value, 1U);
}


double *SubtractionManager::getData() const
{
	return _data;
}

void SubtractionManager::onDataUpdate()
{
	if(_bypass) return;
	_estimation->onDataUpdate();
	_subtraction->onDataUpdate();
}

double *SubtractionManager::getNoisyData()
{
	return _origData;
}

unsigned int SubtractionManager::getLength() const
{
	return _tabLength;
}

unsigned int SubtractionManager::readFile(const char *str)
{
	std::ifstream ifile(str, std::ios_base::ate | std::ios_base::binary);
	_tabLength = ((unsigned int) ifile.tellg()) / (sizeof(short) / sizeof(char));
	ifile.clear();
	ifile.seekg(0, std::ios_base::beg);

	delete[] _origData;
	delete[] _data;
	_origData = new double[_tabLength];
	_data = new double[_tabLength];

	unsigned int pos = 0;
	short sample;

	while (ifile.read((char *)&sample, sizeof(short)))
	{
		_origData[pos++] = MathUtil::ShortToDouble(sample);
	}

	ifile.close();
	_dataSource = DataSource::File;
	return _tabLength;
}

unsigned int SubtractionManager::readBuffer(const short *buffer, const unsigned int length)
{
	if(_bypass) return length;

	_tabLength = length;

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

void SubtractionManager::writeBuffer(short * const buffer) const
{
	if(_bypass) return;
	std::transform(_data, _data + _tabLength, buffer, MathUtil::DoubleToShort);

	// Julius accepts only big-endian raw files but it seems internal buffers
	// are little-endian so no need to convert.
	// std::transform(buffer, buffer + tab_length, buffer,
	//                [] (short val) {return (val << 8) | ((val >> 8) & 0xFF)});
}

void SubtractionManager::copyInputSimple(const unsigned int pos)
{
	// Data copying
	if (_fft->size() <= _tabLength - pos)
	{
		std::copy_n(_data + pos, _fft->size(), _fft->input());
	}
	else
	{
		std::copy_n(_data + pos, _tabLength - pos, _fft->input());
		std::fill_n(_fft->input() + _tabLength - pos, _fft->size() - (_tabLength - pos), 0);
	}
}

void SubtractionManager::copyOutputSimple(const unsigned int pos)
{
	auto normalizeFFT = [&](double x) { return x * _fft->normalizationFactor(); };
	if (_fft->size() <= _tabLength - pos)
	{
		std::transform(_fft->output(), _fft->output() + _fft->size(), _data + pos, normalizeFFT);
	}
	else //fileSize - pos < fftSize
	{
		std::transform(_fft->output(), _fft->output() + _tabLength - pos, _data + pos, normalizeFFT);
	}
}

void SubtractionManager::copyInputOLA(const unsigned int pos)
{
	// Data copying
	if (_ola_frame_increment <= _tabLength - pos) // last case
	{
		std::copy_n(_data + pos, _ola_frame_increment, _fft->input());
		std::fill_n(_fft->input() + _ola_frame_increment, _ola_frame_increment, 0);

		std::fill_n(_data + pos, _ola_frame_increment, 0);
	}
	else
	{
		std::copy_n(_data + pos, _tabLength - pos, _fft->input());
		std::fill_n(_fft->input() + _tabLength - pos, _ola_frame_increment - (_tabLength - pos), 0);

		std::fill_n(_data + pos, _tabLength - pos, 0);
	}
}

void SubtractionManager::copyOutputOLA(const unsigned int pos)
{
	// Lock here
	//ola_mutex.lock();
	for (auto j = 0U; (j < _fft->size()) && (pos + j < _tabLength); ++j)
	{
		_data[pos + j] += _fft->output()[j] / _fft->size();
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

void SubtractionManager::setOLA(const bool val)
{
	_useOLA = val;
}


SubtractionManager::DataSource SubtractionManager::dataSource() const
{
	return _dataSource;
}

Estimation *SubtractionManager::getEstimationImplementation() const
{
	return _estimation.get();
}

void SubtractionManager::setEstimationImplementation(Estimation * value)
{
	_estimation.reset(value);
	_estimation->onFFTSizeUpdate();
}

bool SubtractionManager::bypass()
{
	return _bypass;
}


unsigned int SubtractionManager::getFrameIncrement() const
{
	return _useOLA? _ola_frame_increment : _std_frame_increment;
}

Subtraction *SubtractionManager::getSubtractionImplementation() const
{
	return _subtraction.get();
}

void SubtractionManager::setSubtractionImplementation(Subtraction *value)
{
	_subtraction.reset(value);
	_subtraction->onFFTSizeUpdate();
}

unsigned int SubtractionManager::getSamplingRate() const
{
	return _samplingRate;
}

void SubtractionManager::setSamplingRate(const unsigned int value)
{
	_samplingRate = value;
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
	std::map<std::string, Estimation*> noise_est
	{
		std::make_pair("std", new SimpleEstimation(*this)),
		std::make_pair("martin", new MartinEstimation(*this)),
		std::make_pair("wavelets", new WaveletEstimation(*this))
	};

	enum Algorithm { Standard, EqualLoudness, GeometricApproach, Learning, Bypass};
	static const std::map<std::string, Algorithm> algo
	{
		std::make_pair("std", Algorithm::Standard),
		std::make_pair("el", Algorithm::EqualLoudness),
		std::make_pair("ga", Algorithm::GeometricApproach),
		std::make_pair("bypass", Algorithm::Bypass)
	};

	std::ifstream f("subtraction.conf");
	double alpha, beta, alphawt, betawt;
	unsigned int iterations_tmp;
	std::string noise_alg, alg;

	// Class members
	f >> alpha;
	f >> beta;
	f >> alphawt;
	f >> betawt;
	f >> iterations_tmp;

	// Local stuff
	f >> noise_alg;
	f >> alg;
	f.close();

	enableOLA();

	setIterations(iterations_tmp);

	if (noise_est.find(noise_alg) != noise_est.end())
		setEstimationImplementation(noise_est[noise_alg]);
	else
		std::cerr << "Invalid estimation algorithm";


	if (algo.find(alg) != algo.end())
	{
		switch(algo.at(alg))
		{
			case Algorithm::Standard:
			{
				// Check for memory leak.
				SimpleSpectralSubtraction* subtraction = new SimpleSpectralSubtraction(*this);
				subtraction->setAlpha(alpha);
				subtraction->setBeta(beta);
				setSubtractionImplementation(subtraction);
				break;
			}
			case Algorithm::EqualLoudness:
			{
				EqualLoudnessSpectralSubtraction* subtraction = new EqualLoudnessSpectralSubtraction(*this);
				subtraction->setAlpha(alpha);
				subtraction->setBeta(beta);
				subtraction->setAlphawt(alphawt);
				subtraction->setBetawt(betawt);
				setSubtractionImplementation(subtraction);
				break;
			}
			case Algorithm::GeometricApproach:
			{
				setSubtractionImplementation(new GeometricSpectralSubtraction(*this));
				break;
			}
			case Algorithm::Bypass:
			case Algorithm::Learning:
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
	return _fft->size();
}

void SubtractionManager::setFftSize(unsigned int value)
{
	_fft->updateSize(value);

	onFFTSizeUpdate();
}

unsigned int SubtractionManager::spectrumSize() const
{
	return _fft->spectrumSize();
}
