#include "spectral_subtractor.h"
#include "subtractionconfiguration.h"

#include <subtraction/subtraction_algorithm.h>
#include <estimation/estimation_algorithm.h>


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
		fftw_execute(config.plan_fw_windowed); // A utiliser dans martinEstimation si GA ? normalement, va avec OLA.
		used_spectrum = config.windowed_spectrum;
	}

	(*config.estimation).operator ()(used_spectrum);
}



void SpectralSubtractor::execute(SubtractionConfiguration &config)
{
	// Some configuration and cleaning according to the parameters used
	if (config.getSubtractionImplementation()->algorithm == SubtractionAlgorithm::Algorithm::Bypass) return;
	if (config.datasource == SubtractionConfiguration::DataSource::File)
	{
		config.initDataArray();
	}

	// Maybe make people able to choose by themselves ?
	config.useOLA = config.getSubtractionImplementation()->algorithm == SubtractionAlgorithm::Algorithm::GeometricApproach;
	// Should maybe imply the windowed stuff ?
	const int increment = config.useOLA ? config.ola_frame_increment : config.frame_increment;



	// Execution of the algortihm
	for (auto iter = 0U; iter < config._iterations; ++iter)
	{
		for (auto sample_n = 0U; sample_n < config.tab_length; sample_n += increment)
		{
			// Data copying from input to buffer
			config.copyInput(sample_n);

			// FFT
			fftw_execute(config.plan_fw);

			// TODO : do something good when working on buffers for local data.
			// Noise estimation
			if(config.datasource == SubtractionConfiguration::DataSource::File && sample_n == 0)
				config.onDataUpdate();

			// Noise estimation
			estimationHandler(config);

			// Spectral subtraction
			subtractionHandler(config);

			// IFFT
			fftw_execute(config.plan_bw);

			// Data copying from buffer to output
			config.copyOutput(sample_n);
		}
	}
}
