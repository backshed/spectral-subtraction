// C headers
#include <cmath>

#include <clocale>

// C++ headers
#include <iostream>
#include <parallel/algorithm>

#include "spectral_subtractor.h"
#include "defines.h"
#include "noise_estimator.h"
#include "subtractionconfiguration.h"
// Qt
#include <QDebug>


// Does the main subtraction algorithm
void SpectralSubtractor::subtraction(SubtractionConfiguration& config, fftw_complex *input_spectrum, double* noise_power)
{
	for(unsigned int i = 0; i < spectrumSize; ++i)
	{
		double Apower, Bpower, magnitude, phase, y;

		y = pow(input_spectrum[i][0], 2) + pow(input_spectrum[i][1], 2);
		Apower = y - config.alpha * noise_power[i]; // subtraction
		Bpower = config.beta * y;

		magnitude = sqrt((Apower > Bpower)? Apower : Bpower);
		phase = atan2(input_spectrum[i][1], input_spectrum[i][0]);

		input_spectrum[i][0] = magnitude * cos(phase);
		input_spectrum[i][1] = magnitude * sin(phase);
	}
}

// Does the main subtraction algorithm
void SpectralSubtractor::subtraction_el(SubtractionConfiguration &config, fftw_complex* input_spectrum, double* noise_power)
{
	for(unsigned int i = 0; i < spectrumSize; ++i)
	{
		double Apower, Bpower, magnitude, phase, y, alpha, beta;
		alpha = config.alpha - config.alphawt * (loudness_contour[i] - 60);
		beta  = config.beta  - config.betawt  * (loudness_contour[i] - 60);

		y = pow(input_spectrum[i][0], 2) + pow(input_spectrum[i][1], 2);
		Apower = y - alpha * noise_power[i]; // subtraction
		Bpower = beta * y;

		magnitude =  sqrt((Apower > Bpower)? Apower : Bpower);
		phase = atan2(input_spectrum[i][1], input_spectrum[i][0]);

		input_spectrum[i][0] = magnitude * cos(phase);
		input_spectrum[i][1] = magnitude * sin(phase);
	}
}


// Geometric Approach Algorithm
void SpectralSubtractor::geom_algo(fftw_complex* input_spectrum, double* noise_power, bool firstcall)
{
	static const double geom_alpha = 0.98, geom_beta = 0.98;
	static const double twentysixdb = pow(10., -26./20.);
	static const double thirteendb = pow(10., -20./20.);

	double gammai, gamma, chi, h, ymag, xmag;

	// TODO A CHANGER SI SPECTRUMSIZE CHANGE

	if(firstcall)
	{
		for(unsigned int i = 0; i < spectrumSize; ++i)
			prev_gamma[i] = prev_halfchi[i] = 1;
	}

	for(unsigned int i = 0; i < spectrumSize; ++i)
	{
		// 1) Magnitude spectrum
		ymag = sqrt(pow(input_spectrum[i][0], 2.0) + pow(input_spectrum[i][1], 2.0));

		// 3) compute Gamma
		gammai = pow(ymag, 2.0) / noise_power[i];
		gammai = (thirteendb > gammai)? thirteendb : gammai;

		gamma = geom_beta * prev_gamma[i] + (1.0 - geom_beta) * gammai;
		prev_gamma[i] = gamma;

		// 4) compute Chi

		chi = geom_alpha * prev_halfchi[i] + (1.0 - geom_alpha) * pow(sqrt(gamma) - 1.0, 2.0);
		chi = (twentysixdb > chi)? twentysixdb : chi;

		// 5) compute gain
		h = sqrt( (1.0 - pow(gamma - chi + 1.0, 2.0) / (4.0 * gamma)) / (1.0 - pow(gamma - 1.0 - chi, 2.0) / (4.0 * chi)) );
		h = (h > 1.0)? 1.0 : h;

		// 6) compute enhanced magnitude spectrum
		xmag = h * ymag;
		prev_halfchi[i] = pow(xmag, 2.0) / noise_power[i];

		// 7) reverse FFT
		double orig_phase = atan2(input_spectrum[i][1], input_spectrum[i][0]);
		double orig_amp = xmag;
		input_spectrum[i][0] = orig_amp * cos(orig_phase);
		input_spectrum[i][1] = orig_amp * sin(orig_phase);
	}
}





SpectralSubtractor::SpectralSubtractor():
	prev_gamma(nullptr),
	prev_halfchi(nullptr),
	estimator(new NoiseEstimator(this)),
	loudness_contour(nullptr)
{

}

void SpectralSubtractor::initialize(SubtractionConfiguration& config)
{
	samplingRate = config.samplingRate;
	spectrumSize = config.spectrumSize;
	fftSize = config.fftSize;

	if(loudness_contour != nullptr) delete loudness_contour;
	loadLoudnessContour(config);

	if(prev_gamma != nullptr) delete prev_gamma;
	if(prev_halfchi != nullptr) delete prev_halfchi;
	prev_gamma = new double[config.spectrumSize];
	prev_halfchi = new double[config.spectrumSize];
	estimator->clean();
	estimator->initialize(config);
}

// TODO C++ PLIZ
void SpectralSubtractor::loadLoudnessContour(SubtractionConfiguration& config)
{
	// loading data for loudness contour algo
#ifdef __linux__
	setlocale(LC_ALL, "POSIX"); // THANK YOU LINUX
#endif

	// NOTE : the loudnes_xxx (ex. : xxx = 512)
	// refers to a spectrum with symmetrical coefficients
	// but fftw only compute non-symmetric part, so we only have to read one half of the file.
	// We choose to read the first half, hence it is in reverse order.
	char path[30];
	sprintf(path, "60phon/loudness_%d.data", config.fftSize);
	FILE* ldata = fopen(path, "r");
	loudness_contour = (double*) malloc(sizeof(double) * config.fftSize / 2 );
	for(int i = config.fftSize / 2 - 1; i >= 0 ; --i)
	{
		fscanf( ldata, "%lf\n", &loudness_contour[i] );
	}
	fclose(ldata);
}

SpectralSubtractor::~SpectralSubtractor()
{
	delete[] prev_gamma;
	delete[] prev_halfchi;

	/***/
	delete estimator;
	delete[] loudness_contour;
}

void SpectralSubtractor::subtractionHandler(SubtractionConfiguration &config, bool reinit)
{
	double *effective_power_estimation = 0;
	if(config.noiseEstimationAlgorithm == SubtractionConfiguration::NoiseEstimationAlgorithm::Wavelets)
		effective_power_estimation = config.noise_power_reest;
	else
		effective_power_estimation = config.noise_power;

	// Main algorithm
	switch (config.spectralSubtractionAlgorithm)
	{//TODO optimiser les appels laule
		case SubtractionConfiguration::SpectralSubtractionAlgorithm::Standard:
			subtraction(config, config.spectrum, effective_power_estimation);
			break;
		case SubtractionConfiguration::SpectralSubtractionAlgorithm::EqualLoudness:
			subtraction_el(config, config.spectrum, effective_power_estimation);
			break;
		case SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach:
			geom_algo(config.spectrum, effective_power_estimation, reinit);
			break;
		default:
			throw "critical";
	}
}

void SpectralSubtractor::execute(SubtractionConfiguration &config)
{
	if(config.spectralSubtractionAlgorithm == SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach)
		config.useOLA = true;

	const int increment = config.useOLA? config.ola_frame_increment : config.frame_increment;

	for(int iter = 0; iter < config.iterations; ++iter)
	{
		for(unsigned int sample_n = 0; sample_n < config.filesize; sample_n += increment)
		{

			// Data copying
			if(config.useOLA)
				config.copyInputOLA(sample_n);
			else
				config.copyInputSimple(sample_n);

			// FFT
			fftw_execute(config.plan_fw);

			//Noise estimation
			estimator->estimationHandler(config, sample_n == 0);

			//Spectral subtraction
			subtractionHandler(config, sample_n == 0);

			//**** Result output ****//
			fftw_execute(config.plan_bw);
			if(config.useOLA)
				config.copyOutputOLA(sample_n);
			else
				config.copyOutputSimple(sample_n);

		}
	}
}
