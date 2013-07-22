#ifndef NOISE_ESTIMATION_H
#define NOISE_ESTIMATION_H
#include <fftw3.h>
#include "defines.h"
#include "spectral_subtractor.h"
/*
 *Faire un DataManager avec lequel NoiseEstimator et SpectralSubtractor peuvent accéder, et englober le tout par une méta-classe
 */
class NoiseEstimator
{
		SpectralSubtractor *parent;
		CWTNoiseEstimator cwt_noise_estimator;

	public:
		NoiseEstimator(SpectralSubtractor* parent);
		void writeSimpleCWT(double* signal);

		void estimationHandler(SubtractionConfiguration &config, bool reinit);

		void initialize(SubtractionConfiguration &config);
		void clean();
	private:
		bool waveletEstimation(SubtractionConfiguration &config, bool reinit);
		bool simpleEstimation(fftw_complex* spectrum, double* noise_power, bool reinit);
		void martinEstimation(fftw_complex* in, int nrf, double *x, double tinc, bool reinit);

		void compute_power_and_phase(fftw_complex* in, double* powoutput, double* phaseoutput);
		int update_noise(fftw_complex* in, double* old_rms);
		void compute_power(fftw_complex *in, double *powoutput);

		uint fftSize;
		uint spectrumSize;
		uint samplingRate;
};



#endif // NOISE_ESTIMATION_H
