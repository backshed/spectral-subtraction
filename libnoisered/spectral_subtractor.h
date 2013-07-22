#ifndef _NOISE_RED_H
#define _NOISE_RED_H
/*******************************
 * This file is the header
 * for the different functions
 * used in spectral subtraction
 * noise-reduction algorithms.
 *
 * Written by Jean-Mickael Celerier
 * for the ASPL.
 *******************************/
#include "fftw3.h"
#include "defines.h"
#include "cwt_noise_estimator.h"

class NoiseEstimator;
class SubtractionConfiguration;
class SpectralSubtractor
{
		friend class NoiseEstimator;

	public:
		SpectralSubtractor();
		~SpectralSubtractor();

		void execute(SubtractionConfiguration& config);

		void initialize(SubtractionConfiguration &config);
	private:
		void subtraction(SubtractionConfiguration &config, fftw_complex* input_spectrum, double* noise_power);
		void subtraction_el(SubtractionConfiguration &config, fftw_complex* input_spectrum, double* noise_power);
		void geom_algo(fftw_complex* in, double* noise_power, bool firstcall);

	private:
		/*** For geom algo ***/
		double* prev_gamma;
		double* prev_halfchi;

		/***/
		NoiseEstimator* estimator;
		double* loudness_contour;
		void subtractionHandler(SubtractionConfiguration &config, bool reinit);
		void loadLoudnessContour(SubtractionConfiguration& config);

		uint fftSize;
		uint spectrumSize;
		uint samplingRate;
};



#endif
