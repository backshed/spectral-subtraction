#include "spectral_subtractor.h"
#include "subtractionconfiguration.h"

#include <subtraction/subtraction_algorithm.h>
#include <estimation/estimation_algorithm.h>


void SpectralSubtractor::subtractionHandler(SubtractionConfiguration &config)
{
	(*config.getSubtractionImplementation())(config.spectrum(), config.getEstimationImplementation()->noisePower());
}

void SpectralSubtractor::estimationHandler(SubtractionConfiguration &config) // Reinit pour la CWT
{

	(*config.getEstimationImplementation())(config.spectrum());
}



void SpectralSubtractor::execute(SubtractionConfiguration &config)
{
	// Some configuration and cleaning according to the parameters used
	if (config.bypass()) return;
	if (config.dataSource() == SubtractionConfiguration::DataSource::File)
	{
		config.initDataArray();
	}
	// For Julius, call onDataUpdate() on every file change, and only once if it is mic input.

	// Execution of the algortihm
	for (auto iter = 0U; iter < config.iterations(); ++iter)
	{
		for (auto sample_n = 0U; sample_n < config.getLength(); sample_n += config.getFrameIncrement())
		{
			// Data copying from input to buffer
			config.copyInput(sample_n);

			// FFT
			config.forwardFFT();

			// Noise estimation
			if(config.dataSource() == SubtractionConfiguration::DataSource::File && sample_n == 0)
				config.onDataUpdate();

			// Noise estimation
			estimationHandler(config);

			// Spectral subtraction
			subtractionHandler(config);

			// IFFT
			config.backwardFFT();

			// Data copying from buffer to output
			config.copyOutput(sample_n);
		}
	}
}
