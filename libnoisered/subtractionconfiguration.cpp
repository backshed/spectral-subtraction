#include "subtractionconfiguration.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>

SubtractionConfiguration::SubtractionConfiguration(int fft_Size, int sampling_Rate)
{
	data = nullptr;
	origdata = nullptr;
	tab_length = 0;

	fftSize = fft_Size;
	spectrumSize = fft_Size / 2 + 1;
	samplingRate = sampling_Rate;

	ola_frame_increment = fftSize / 2;
	frame_increment = fftSize;

	initStructs();
}

void SubtractionConfiguration::initStructs()
{
	in = fftw_alloc_real(fftSize);
	windowed_in = fftw_alloc_real(fftSize);
	out = fftw_alloc_real(fftSize);
	tmp_out = fftw_alloc_real(fftSize);

	spectrum = fftw_alloc_complex(spectrumSize);
	tmp_spectrum = fftw_alloc_complex(spectrumSize);
	windowed_spectrum = fftw_alloc_complex(spectrumSize);

	noise_power = new double[fftSize];
	noise_power_reest = new double[fftSize];

	// Initialize the fftw plans (so.. FAAAST)
	plan_fw = fftw_plan_dft_r2c_1d(fftSize, in, spectrum, FFTW_ESTIMATE);
	plan_fw_windowed = fftw_plan_dft_r2c_1d(fftSize, windowed_in, windowed_spectrum, FFTW_ESTIMATE);
	plan_bw = fftw_plan_dft_c2r_1d(fftSize, spectrum, out, FFTW_ESTIMATE);
	plan_bw_temp = fftw_plan_dft_c2r_1d(fftSize, tmp_spectrum, tmp_out, FFTW_ESTIMATE);
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

void SubtractionConfiguration::clean()
{
	fftw_free(in);
	fftw_free(windowed_in);
	fftw_free(out);
	fftw_free(tmp_out);
	fftw_free(tmp_spectrum);
	fftw_free(spectrum);
	fftw_free(windowed_spectrum);

	delete[] noise_power;
	delete[] noise_power_reest;

}

SubtractionConfiguration::~SubtractionConfiguration()
{
	clean();

	delete data;
	delete origdata;
}


void SubtractionConfiguration::reinitData()
{
	std::copy(origdata, origdata + tab_length, data);
}

double SubtractionConfiguration::getAlpha() const
{
	return alpha;
}

void SubtractionConfiguration::setAlpha(double value)
{
	alpha = std::max(value, 0.000001);
}

double SubtractionConfiguration::getBeta() const
{
	return beta;
}

void SubtractionConfiguration::setBeta(double value)
{
	beta = std::max(value, 0.000001);
}
double SubtractionConfiguration::getAlphawt() const
{
	return alphawt;
}

void SubtractionConfiguration::setAlphawt(double value)
{
	alphawt = std::max(value, 0.000001);
}
double SubtractionConfiguration::getBetawt() const
{
	return betawt;
}

void SubtractionConfiguration::setBetawt(double value)
{
	betawt = std::max(value, 0.000001);
}

int SubtractionConfiguration::getIterations() const
{
	return iterations;
}

void SubtractionConfiguration::setIterations(int value)
{
	iterations = std::max(value, 1);
}

SubtractionConfiguration::NoiseEstimationAlgorithm SubtractionConfiguration::getNoiseEstimationAlgorithm() const
{
	return estimationAlgo;
}

void SubtractionConfiguration::setNoiseEstimationAlgorithm(const NoiseEstimationAlgorithm &value)
{
	estimationAlgo = value;
}

SubtractionConfiguration::SpectralSubtractionAlgorithm SubtractionConfiguration::getSpectralSubtractionAlgorithm() const
{
	return subtractionAlgo;
}

void SubtractionConfiguration::setSpectralSubtractionAlgorithm(const SpectralSubtractionAlgorithm &value)
{
	subtractionAlgo = value;
}

double *SubtractionConfiguration::getData()
{
	return data;
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

	if (origdata != nullptr) delete origdata;
	if (data != nullptr) delete data;
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
	return tab_length;
}


unsigned int SubtractionConfiguration::readBuffer(short *buffer, int length)
{
	tab_length = length;
	if (origdata != nullptr) delete origdata;
	if (data != nullptr) delete data;
	origdata = new double[tab_length];
	data = new double[tab_length];

	std::transform(buffer, buffer + tab_length, origdata, &SubtractionConfiguration::ShortToDouble);

	return tab_length;
}

void SubtractionConfiguration::writeBuffer(short *buffer)
{
	std::transform(data, data + tab_length, buffer, &SubtractionConfiguration::DoubleToShort);
}

void SubtractionConfiguration::copyInputSimple(int pos)
{
	// Data copying
	if (fftSize <= tab_length - pos)
	{
		std::copy_n(data + pos, fftSize, in);
	}
	else
	{
		std::copy_n(data + pos, tab_length - pos, in);
		std::fill_n(in + tab_length - pos, fftSize - (tab_length - pos), 0);
	}
}

void SubtractionConfiguration::copyOutputSimple(int pos)
{
	auto normalizeFFT = [&](double x) { return x / fftSize; };
	if (fftSize <= tab_length - pos)
	{
		std::transform(out, out + fftSize, data + pos, normalizeFFT);
	}
	else //fileSize - pos < fftSize
	{
		std::transform(out, out + tab_length - pos, data + pos, normalizeFFT);
	}
}

void SubtractionConfiguration::copyInputOLA(int pos)
{
	// Data copying
	if (ola_frame_increment <= tab_length - pos) // last case
	{
		std::copy_n(data + pos, ola_frame_increment, in);
		std::fill_n(in + ola_frame_increment, ola_frame_increment, 0);

		std::copy_n(in, fftSize, windowed_in);
		std::fill_n(data + pos, ola_frame_increment, 0);
	}
	else
	{
		std::copy_n(data + pos, tab_length - pos, in);
		std::fill_n(in + tab_length - pos, ola_frame_increment - (tab_length - pos), 0);

		std::copy_n(in, fftSize, windowed_in);
		std::fill_n(data + pos, tab_length - pos, 0);
	}
}

void SubtractionConfiguration::copyOutputOLA(int pos)
{
	// Lock here
	//ola_mutex.lock();
	for (unsigned int j = 0; (j < fftSize) && (pos + j < tab_length); ++j)
	{
		data[pos + j] += out[j] / fftSize;
	}
	// Unlock here
	//ola_mutex.unlock();
}
unsigned int SubtractionConfiguration::getSamplingRate() const
{
	return samplingRate;
}

void SubtractionConfiguration::setSamplingRate(unsigned int value)
{
	samplingRate = value;
	clean();
	initStructs();
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
	static const std::map<std::string, NoiseEstimationAlgorithm> noise_est
	{
		std::make_pair("std", NoiseEstimationAlgorithm::Simple),
		std::make_pair("martin", NoiseEstimationAlgorithm::Martin),
		std::make_pair("wavelets", NoiseEstimationAlgorithm::Wavelets)
	};
	static const std::map<std::string, SpectralSubtractionAlgorithm> algo
	{
		std::make_pair("std", SpectralSubtractionAlgorithm::Standard),
		std::make_pair("el", SpectralSubtractionAlgorithm::EqualLoudness),
		std::make_pair("ga", SpectralSubtractionAlgorithm::GeometricApproach)
	};

	std::ifstream f("subtraction.conf");
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

	if (noise_est.find(noise_alg) != noise_est.end())
		estimationAlgo = noise_est.at(noise_alg);
	else
		std::cerr << "Invalid noise estimation algorithm";

	if (algo.find(alg) != algo.end())
		subtractionAlgo = algo.at(alg);
	else
		std::cerr << "Invalid subtraction algorithm";

}

unsigned int SubtractionConfiguration::getFftSize() const
{
	return fftSize;
}

void SubtractionConfiguration::setFftSize(unsigned int value)
{
	fftSize = value;
	spectrumSize = value / 2 + 1;
	clean();
	initStructs();
}

unsigned int SubtractionConfiguration::getSpectrumSize() const
{
	return spectrumSize;
}
