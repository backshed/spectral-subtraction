#include "subtractionconfiguration.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>

#include <climits>

#include <estimation/estimation_algorithm.h>
#include <subtraction/subtraction_algorithm.h>

SubtractionConfiguration::SubtractionConfiguration(int fft_Size, int sampling_Rate):
	_fftSize(fft_Size),
	_samplingRate(sampling_Rate),
	data(nullptr),
	origdata(nullptr),
	tab_length(0)
{
	onFFTSizeUpdate();
	onDataUpdate();
}





void SubtractionConfiguration::onFFTSizeUpdate()
{
	_spectrumSize = _fftSize / 2 + 1;
	ola_frame_increment = _fftSize / 2;
	frame_increment = _fftSize;

	in = fftw_alloc_real(_fftSize);
	out = fftw_alloc_real(_fftSize);

	spectrum = fftw_alloc_complex(_spectrumSize);

	// To move ?
	windowed_in = fftw_alloc_real(_fftSize);
	windowed_spectrum = fftw_alloc_complex(_spectrumSize);
	plan_fw_windowed = fftw_plan_dft_r2c_1d(_fftSize, windowed_in, windowed_spectrum, FFTW_ESTIMATE);

	// Initialize the fftw plans (so.. FAAAST)
	plan_fw = fftw_plan_dft_r2c_1d(_fftSize, in, spectrum, FFTW_ESTIMATE);
	plan_bw = fftw_plan_dft_c2r_1d(_fftSize, spectrum, out, FFTW_ESTIMATE);

	estimation->onFFTSizeUpdate();
	subtraction->onFFTSizeUpdate();
}

double SubtractionConfiguration::ShortToDouble(short x)
{
	static const double normalizationFactor = 1.0 / pow(2.0, sizeof(short) * 8 - 1.0);
	return x * normalizationFactor;
}

short SubtractionConfiguration::DoubleToShort(double x)
{
	static const double denormalizationFactor = pow(2.0, sizeof(short) * 8 - 1.0);
	return x * denormalizationFactor;
}

void SubtractionConfiguration::copyInput(unsigned int pos)
{
	if(useOLA)
		copyInputOLA(pos);
	else
		copyInputSimple(pos);
}

void SubtractionConfiguration::copyOutput(unsigned int pos)
{
	if(useOLA)
		copyOutputOLA(pos);
	else
		copyOutputSimple(pos);
}

void SubtractionConfiguration::clean()
{
	fftw_free(in);
	fftw_free(windowed_in);
	fftw_free(out);

	fftw_free(spectrum);
	fftw_free(windowed_spectrum);





}

SubtractionConfiguration::~SubtractionConfiguration()
{
	clean();

	delete subtraction;
	delete estimation;

	delete[] data;
	delete[] origdata;
}


void SubtractionConfiguration::initDataArray()
{
	std::copy(origdata, origdata + tab_length, data);
}

int SubtractionConfiguration::iterations() const
{
	return _iterations;
}

void SubtractionConfiguration::setIterations(int value)
{
	_iterations = std::max(value, 1);
}


double *SubtractionConfiguration::getData()
{
	return data;
}

void SubtractionConfiguration::onDataUpdate()
{
	estimation->onDataUpdate();
	subtraction->onDataUpdate();
}

double *SubtractionConfiguration::getNoisyData()
{
	return origdata;
}

unsigned int SubtractionConfiguration::getSize()
{
	return tab_length;
}

unsigned int SubtractionConfiguration::readFile(char *str)
{
	std::ifstream ifile(str, std::ios_base::ate | std::ios_base::binary);
	tab_length = ifile.tellg() / (sizeof(short) / sizeof(char));
	ifile.clear();
	ifile.seekg(0, std::ios_base::beg);

	delete[] origdata;
	delete[] data;
	origdata = new double[tab_length];
	data = new double[tab_length];

	// We have to get signal between -1 and 1
	static const double normalizationFactor = pow(2.0, sizeof(short) * 8 - 1.0);

	unsigned int pos = 0;
	short sample;
	while (ifile.read((char *)&sample, sizeof(short)) && pos < tab_length)
	{
		origdata[pos++] = sample / normalizationFactor;
	}

	ifile.close();
	datasource = DataSource::File;
	return tab_length;
}

unsigned int SubtractionConfiguration::readBuffer(short *buffer, int length)
{
	if(subtraction->algorithm == SubtractionAlgorithm::Algorithm::Bypass) return length;

	tab_length = length;
	delete[] origdata;
	delete[] data;
	origdata = new double[tab_length];
	data = new double[tab_length];

	// Julius accepts only big-endian raw files but it seems internal buffers
	// are little-endian so no need to convert.
	// std::transform(buffer, buffer + tab_length, buffer,
	//                [] (short val) {return (val << 8) | ((val >> 8) & 0xFF)});

	std::transform(buffer, buffer + tab_length, origdata, &SubtractionConfiguration::ShortToDouble);
	initDataArray();

	datasource = DataSource::Buffer;
	return tab_length;
}

void SubtractionConfiguration::writeBuffer(short *buffer)
{
	std::transform(data, data + tab_length, buffer, &SubtractionConfiguration::DoubleToShort);
	// Julius accepts only big-endian raw files but it seems internal buffers
	// are little-endian so no need to convert.
	// std::transform(buffer, buffer + tab_length, buffer,
	//                [] (short val) {return (val << 8) | ((val >> 8) & 0xFF)});
}

void SubtractionConfiguration::copyInputSimple(unsigned int pos)
{
	// Data copying
	if (_fftSize <= tab_length - pos)
	{
		std::copy_n(data + pos, _fftSize, in);
	}
	else
	{
		std::copy_n(data + pos, tab_length - pos, in);
		std::fill_n(in + tab_length - pos, _fftSize - (tab_length - pos), 0);
	}
}

void SubtractionConfiguration::copyOutputSimple(unsigned int pos)
{
	auto normalizeFFT = [&](double x) { return x / _fftSize; };
	if (_fftSize <= tab_length - pos)
	{
		std::transform(out, out + _fftSize, data + pos, normalizeFFT);
	}
	else //fileSize - pos < fftSize
	{
		std::transform(out, out + tab_length - pos, data + pos, normalizeFFT);
	}
}

void SubtractionConfiguration::copyInputOLA(unsigned int pos)
{
	// Data copying
	if (ola_frame_increment <= tab_length - pos) // last case
	{
		std::copy_n(data + pos, ola_frame_increment, in);
		std::fill_n(in + ola_frame_increment, ola_frame_increment, 0);

		std::copy_n(in, _fftSize, windowed_in);
		std::fill_n(data + pos, ola_frame_increment, 0);
	}
	else
	{
		std::copy_n(data + pos, tab_length - pos, in);
		std::fill_n(in + tab_length - pos, ola_frame_increment - (tab_length - pos), 0);

		std::copy_n(in, _fftSize, windowed_in);
		std::fill_n(data + pos, tab_length - pos, 0);
	}
}

void SubtractionConfiguration::copyOutputOLA(unsigned int pos)
{
	// Lock here
	//ola_mutex.lock();
	for (auto j = 0U; (j < _fftSize) && (pos + j < tab_length); ++j)
	{
		data[pos + j] += out[j] / _fftSize;
	}
	// Unlock here
	//ola_mutex.unlock();
}

unsigned int SubtractionConfiguration::getFrameIncrement()
{
	return useOLA? ola_frame_increment : frame_increment;
}

SubtractionAlgorithm *SubtractionConfiguration::getSubtractionImplementation() const
{
	return subtraction;
}

void SubtractionConfiguration::setSubtractionImplementation(SubtractionAlgorithm *value)
{//TODO
	subtraction = value;
}

unsigned int SubtractionConfiguration::getSamplingRate() const
{
	return _samplingRate;
}

void SubtractionConfiguration::setSamplingRate(unsigned int value)
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
void SubtractionConfiguration::readParametersFromFile()
{
//	static const std::map<std::string, NoiseEstimationAlgorithm> noise_est
//	{
//		std::make_pair("std", NoiseEstimationAlgorithm::Simple),
//		std::make_pair("martin", NoiseEstimationAlgorithm::Martin),
//		std::make_pair("wavelets", NoiseEstimationAlgorithm::Wavelets)
//	};
//	static const std::map<std::string, SubtractionAlgorithm::Algorithm> algo
//	{
//		std::make_pair("std", SubtractionAlgorithm::Algorithm::Standard),
//		std::make_pair("el", SubtractionAlgorithm::Algorithm::EqualLoudness),
//		std::make_pair("ga", SubtractionAlgorithm::Algorithm::GeometricApproach),
//		std::make_pair("bypass", SubtractionAlgorithm::Algorithm::Bypass)
//	};

//	std::ifstream f("subtraction.conf");
//	std::string noise_alg, alg;

//	// Class members
//	f >> _alpha;
//	f >> _beta;
//	f >> _alphawt;
//	f >> _betawt;
//	f >> _iterations;

//	// Local stuff
//	f >> noise_alg;
//	f >> alg;
//	f.close();

//	if (noise_est.find(noise_alg) != noise_est.end())
//		estimationAlgo = noise_est.at(noise_alg);
//	else
//		std::cerr << "Invalid noise estimation algorithm";

//	if (algo.find(alg) != algo.end())
//		subtractionAlgo = algo.at(alg);
//	else
//		std::cerr << "Invalid subtraction algorithm";

}

unsigned int SubtractionConfiguration::FFTSize() const
{
	return _fftSize;
}

void SubtractionConfiguration::setFftSize(unsigned int value)
{
	_fftSize = value;
	clean();
	onFFTSizeUpdate();
}

unsigned int SubtractionConfiguration::spectrumSize() const
{
	return _spectrumSize;
}


