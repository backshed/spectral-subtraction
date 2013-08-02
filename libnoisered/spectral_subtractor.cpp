// C headers
#include <cmath>
#include <clocale>

// C++ headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "spectral_subtractor.h"
#include "defines.h"
#include "noise_estimator.h"
#include "subtractionconfiguration.h"
// Qt


// Does the main subtraction algorithm
void SpectralSubtractor::subtraction(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power)
{
	for (auto i = 0U; i < spectrumSize; ++i)
	{
		double Apower, Bpower, magnitude, phase, y;

		y = pow(input_spectrum[i][0], 2) + pow(input_spectrum[i][1], 2);

		Apower = y - config.alpha * noise_power[i];
		Bpower = config.beta * y;

		magnitude = sqrt((Apower > Bpower) ? Apower : Bpower);
		phase = atan2(input_spectrum[i][1], input_spectrum[i][0]);

		input_spectrum[i][0] = magnitude * cos(phase);
		input_spectrum[i][1] = magnitude * sin(phase);
	}
}

// Does the main subtraction algorithm
void SpectralSubtractor::subtraction_el(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power)
{
	for (auto i = 0U; i < spectrumSize; ++i)
	{
		double Apower, Bpower, magnitude, phase, y, alpha, beta;
		alpha = config.alpha - config.alphawt * (config.loudness_contour[i] - 60);
		beta  = config.beta  - config.betawt  * (config.loudness_contour[i] - 60);

		y = pow(input_spectrum[i][0], 2) + pow(input_spectrum[i][1], 2);

		Apower = y - alpha * noise_power[i];
		Bpower = beta * y;

		magnitude =  sqrt((Apower > Bpower) ? Apower : Bpower);
		phase = atan2(input_spectrum[i][1], input_spectrum[i][0]);

		input_spectrum[i][0] = magnitude * cos(phase);
		input_spectrum[i][1] = magnitude * sin(phase);
	}
}

// Debug function
void SpectralSubtractor::outputNoiseSpectrum(SubtractionConfiguration &config)
{
	std::ofstream out("spectrumList.txt", std::ios_base::app);
	for (auto i = 0U; i < config.spectrumSize; ++i)
		out << config.noise_power[i] << "\n";
	out << std::endl;
	for (auto i = 0U; i < config.spectrumSize; ++i)
		out << config.noise_power_reest[i] << "\n";
	out << std::endl;
	out.close();

}

void SpectralSubtractor::geom_algo(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power)
{
	// OK
	static const double geom_alpha = 0.98, geom_beta = 0.98;
	static const double twentysixdb = pow(10., -26. / 20.);
	static const double thirteendb = pow(10., -20. / 20.);

	double gammai, gamma, chi, h, ymag, xmag;

	for (auto i = 0U; i < spectrumSize; ++i)
	{
		// 1) Magnitude spectrum
		ymag = sqrt(pow(input_spectrum[i][0], 2.0) + pow(input_spectrum[i][1], 2.0));

		// 3) compute Gamma
		gammai = pow(ymag, 2.0) / noise_power[i]; //todo max(thirdeendb, expr)
		gammai = (thirteendb > gammai) ? thirteendb : gammai;

		gamma = geom_beta * config.prev_gamma[i] + (1.0 - geom_beta) * gammai;
		config.prev_gamma[i] = gamma;

		// 4) compute Chi
		chi = geom_alpha * config.prev_halfchi[i] + (1.0 - geom_alpha) * pow(sqrt(gamma) - 1.0, 2.0);
		chi = (twentysixdb > chi) ? twentysixdb : chi;

		// 5) compute gain
		h = sqrt((1.0 - pow(gamma - chi + 1.0, 2.0) / (4.0 * gamma)) / (1.0 - pow(gamma - 1.0 - chi, 2.0) / (4.0 * chi)));
		h = (h > 1.0) ? 1.0 : h;

		// 6) compute enhanced magnitude spectrum
		xmag = h * ymag;
		config.prev_halfchi[i] = pow(xmag, 2.0) / noise_power[i];

		// 7) reverse FFT
		double orig_phase = atan2(input_spectrum[i][1], input_spectrum[i][0]);
		double orig_amp = xmag;
		input_spectrum[i][0] = orig_amp * cos(orig_phase);
		input_spectrum[i][1] = orig_amp * sin(orig_phase);
	}
}



SpectralSubtractor::SpectralSubtractor():
	estimator(new NoiseEstimator(this))
{

}

void SpectralSubtractor::initialize(SubtractionConfiguration &config)
{
	samplingRate = config.samplingRate;
	spectrumSize = config.spectrumSize;
	fftSize = config.fftSize;




	estimator->clean(config);
	estimator->initialize(config);
}



SpectralSubtractor::~SpectralSubtractor()
{
	delete estimator;
}

void SpectralSubtractor::subtractionHandler(SubtractionConfiguration &config)
{
	double *effective_power_estimation = 0;
	if (config.estimationAlgo == SubtractionConfiguration::NoiseEstimationAlgorithm::Wavelets)
		effective_power_estimation = config.noise_power_reest;
	else
		effective_power_estimation = config.noise_power;

	// Main algorithm
	switch (config.subtractionAlgo)
	{
			//TODO optimiser les appels laule
		case SubtractionConfiguration::SpectralSubtractionAlgorithm::Standard:
			subtraction(config, config.spectrum, effective_power_estimation);
			break;

		case SubtractionConfiguration::SpectralSubtractionAlgorithm::EqualLoudness:
			subtraction_el(config, config.spectrum, effective_power_estimation);
			break;

		case SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach:
			geom_algo(config, config.spectrum, effective_power_estimation);
			break;

		default:
			throw "critical";
	}
}

void SpectralSubtractor::execute(SubtractionConfiguration &config)
{
	// Some configuration and cleaning according to the parameters used
	static bool reinit_noise = true;
	if(config.subtractionAlgo == SubtractionConfiguration::SpectralSubtractionAlgorithm::Bypass) return;
	if(config.datasource == SubtractionConfiguration::DataSource::File)
	{
		config.initDataArray();
		reinit_noise = true;
	}

	// Maybe make people able to choose by themselves ?
	config.useOLA = config.subtractionAlgo == SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach;
	const int increment = config.useOLA ? config.ola_frame_increment : config.frame_increment;



	// Execution of the algortihm
	for (auto iter = 0U; iter < config.iterations; ++iter)
	{
		for (auto sample_n = 0U; sample_n < config.tab_length; sample_n += increment)
		{
			// Reinit inner algorithm parameters
			if(reinit_noise || (config.datasource == SubtractionConfiguration::DataSource::File && sample_n == 0) )
			{
				config.noise_est_cfg.reinitialize(config);
				config.initializeAlgorithmData();
			}

			// Data copying from input
			if (config.useOLA)
				config.copyInputOLA(sample_n);
			else
				config.copyInputSimple(sample_n);

			// FFT
			fftw_execute(config.plan_fw);

			// TODO : do something good when working on buffers for local data.
			// Noise estimation
			if(config.datasource == SubtractionConfiguration::DataSource::File)
				estimator->estimationHandler(config, sample_n == 0);
			else
				estimator->estimationHandler(config, reinit_noise);

			//Spectral subtraction
			subtractionHandler(config);

			// IFFT
			fftw_execute(config.plan_bw);

			// Data copying to output
			if (config.useOLA)
				config.copyOutputOLA(sample_n);
			else
				config.copyOutputSimple(sample_n);

			reinit_noise = false;
		}
	}
}
