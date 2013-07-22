#include "subtractionconfiguration.h"
#include <parallel/algorithm>
#include <sys/stat.h>
#include <fstream>

#include <QDebug>

SubtractionConfiguration::SubtractionConfiguration(int fft_Size, int sampling_Rate)
{
	data = nullptr;
	origdata = nullptr;
	filesize = 0;

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
	plan_fw = fftw_plan_dft_r2c_1d ( fftSize, in, spectrum, FFTW_ESTIMATE );
	plan_fw_windowed = fftw_plan_dft_r2c_1d ( fftSize, windowed_in, windowed_spectrum, FFTW_ESTIMATE );
	plan_bw = fftw_plan_dft_c2r_1d ( fftSize, spectrum, out, FFTW_ESTIMATE );
	plan_bw_temp = fftw_plan_dft_c2r_1d ( fftSize, tmp_spectrum, tmp_out, FFTW_ESTIMATE );
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

	delete data;
	delete origdata;
}

SubtractionConfiguration::~SubtractionConfiguration()
{
	clean();
}





void SubtractionConfiguration::reinitData()
{
	std::copy(origdata, origdata + filesize, data);
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

double *SubtractionConfiguration::getNoiseData()
{
	return origdata;
}

unsigned int SubtractionConfiguration::getSize()
{
	return filesize;
}

unsigned int SubtractionConfiguration::readFile(char* str)
{
	std::ifstream ifile(str, std::ios_base::ate | std::ios_base::binary);
	filesize = ifile.tellg() / (sizeof(short) / sizeof(char));
	ifile.clear();
	ifile.seekg(0, std::ios_base::beg);

	if(origdata != 0) delete origdata;
	if(data != 0) delete data;
	origdata = new double[filesize];
	data = new double[filesize];


	// We have to get signal between -1 and 1
	double normalizationFactor = pow(2.0, sizeof(short) * 8 - 1.0);

	unsigned int pos = 0;
	short sample;
	while(ifile.read((char*)&sample, sizeof(short)) && pos < filesize)
	{
		origdata[pos++] = sample / normalizationFactor;
	}

	ifile.close();
	return filesize;
}



void SubtractionConfiguration::copyInputSimple(int pos)
{
	// Data copying
	if(fftSize > filesize - pos) // TODO last case *** INVERT FOR CACHE OPTIMISATION ***
	{
		std::copy_n(data + pos, filesize - pos, in);
		std::fill_n(in + filesize - pos, fftSize - (filesize - pos), 0);
	}
	else
	{
		std::copy_n(data + pos, fftSize, in);
	}
}

void SubtractionConfiguration::copyOutputSimple(int pos)
{
	for(unsigned int j = 0;
		(j < fftSize) && (pos + j < filesize);
		++j)
	{
		data[pos+j] = out[j] / fftSize;
	}
}

void SubtractionConfiguration::copyInputOLA(int pos)
{
	// Data copying
	if(ola_frame_increment > filesize - pos) // last case
	{
		std::copy_n(data + pos, filesize - pos, in);
		std::fill_n(in + filesize - pos, ola_frame_increment - (filesize - pos), 0);

		std::copy_n(in, fftSize, windowed_in);
		std::fill_n(data + pos, filesize - pos, 0);
	}
	else
	{
		std::copy_n(data + pos, ola_frame_increment, in);
		std::fill_n(in + ola_frame_increment, ola_frame_increment, 0);

		std::copy_n(in, fftSize, windowed_in);
		std::fill_n(data + pos, ola_frame_increment, 0);
	}
}
void SubtractionConfiguration::copyOutputOLA(int pos)
{
	for(unsigned int j = 0; (j < fftSize) && (pos + j < filesize); ++j)
	{
		data[pos+j] += out[j] / fftSize;
	}
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
