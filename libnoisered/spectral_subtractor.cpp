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

#include "subtractionconfiguration.h"


#include "subtraction_algorithm.h"
#include "estimation_algorithm.h"
 // Debug function
void SpectralSubtractor::outputNoiseSpectrum(SubtractionConfiguration &config)
{
//	std::ofstream out("spectrumList.txt", std::ios_base::app);
//	for (auto i = 0U; i < config._spectrumSize; ++i)
//		out << config.noise_power[i] << "\n";
//	out << std::endl;
//	for (auto i = 0U; i < config._spectrumSize; ++i)
//		out << config.noise_power_reest[i] << "\n";
//	out << std::endl;
//	out.close();

}

SpectralSubtractor::SpectralSubtractor()
{

}

void SpectralSubtractor::initialize(SubtractionConfiguration &config)
{
	samplingRate = config._samplingRate;
	spectrumSize = config._spectrumSize;
	fftSize = config._fftSize;


	config.estimation->prepare();
//	estimator->clean(config);
//	estimator->initialize(config);
}



SpectralSubtractor::~SpectralSubtractor()
{
}

void SpectralSubtractor::subtractionHandler(SubtractionConfiguration &config)
{
	(*config.subtraction)(config.spectrum, config.estimation->noisePower());
}

void SpectralSubtractor::estimationHandler(SubtractionConfiguration &config) // Reinit pour la CWT
{
	fftw_complex* used_spectrum = config.spectrum;
	if    (config.getSubtractionImplementation()->algorithm == SubtractionAlgorithm::Algorithm::GeometricApproach
		&& config.estimation->algorithm == EstimationAlgorithm::Algorithm::Martin)
	{
		fftw_execute(config.plan_fw_windowed); // A utiliser dans martinEstimation si GA ?
		used_spectrum = config.windowed_spectrum;
	}

	(*config.estimation).operator ()(used_spectrum);
}



void SpectralSubtractor::execute(SubtractionConfiguration &config)
{
	// Some configuration and cleaning according to the parameters used
	static bool reinit_noise = true;
	if (config.getSubtractionImplementation()->algorithm == SubtractionAlgorithm::Algorithm::Bypass) return;
	if (config.datasource == SubtractionConfiguration::DataSource::File)
	{
		config.initDataArray();
		reinit_noise = true;
	}

	// Maybe make people able to choose by themselves ?
	config.useOLA = config.getSubtractionImplementation()->algorithm == SubtractionAlgorithm::Algorithm::GeometricApproach;
	const int increment = config.useOLA ? config.ola_frame_increment : config.frame_increment;



	// Execution of the algortihm
	for (auto iter = 0U; iter < config._iterations; ++iter)
	{
		for (auto sample_n = 0U; sample_n < config.tab_length; sample_n += increment)
		{
			// Reinit inner algorithm parameters
			if(reinit_noise || (config.datasource == SubtractionConfiguration::DataSource::File && sample_n == 0) )
			{
//				config.noise_est_cfg.reinitialize(config);
//				config.initializeAlgorithmData();
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
//			if(config.datasource == SubtractionConfiguration::DataSource::File)
//				//reinit = sample_n == 0
//			else
//				//reinit = reinit_noise

			estimationHandler(config);

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
