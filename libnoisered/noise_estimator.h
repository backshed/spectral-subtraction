#ifndef NOISE_ESTIMATION_H
#define NOISE_ESTIMATION_H
#include <fftw3.h>
#include "defines.h"
#include "spectral_subtractor.h"
/*
 *Faire un DataManager avec lequel NoiseEstimator et SpectralSubtractor peuvent accéder, et englober le tout par une méta-classe
 */
/**
 * @brief
 *
 */
class NoiseEstimator
{
		SpectralSubtractor *parent; /**< TODO */
		CWTNoiseEstimator cwt_noise_estimator; /**< TODO */

	public:
		NoiseEstimator(SpectralSubtractor* parent);
		/**
		 * @brief
		 *
		 * @param signal
		 */
		/**
		 * @brief
		 *
		 */
		void writeSimpleCWT(double* signal);

		/**
		 * @brief
		 *
		 * @param config
		 * @param reinit
		 */
		void estimationHandler(SubtractionConfiguration &config, bool reinit);

		/**
		 * @brief
		 *
		 * @param config
		 */
		void initialize(SubtractionConfiguration &config);
		/**
		 * @brief
		 *
		 */
		void clean();
	private:
		/**
		 * @brief
		 *
		 * @param config
		 * @param reinit
		 * @return bool
		 */
		bool waveletEstimation(SubtractionConfiguration &config, bool reinit);
		/**
		 * @brief
		 *
		 * @param spectrum
		 * @param noise_power
		 * @param reinit
		 * @return bool
		 */
		bool simpleEstimation(fftw_complex* spectrum, double* noise_power, bool reinit);
		/**
		 * @brief
		 *
		 * @param in
		 * @param nrf
		 * @param x
		 * @param tinc
		 * @param reinit
		 */
		void martinEstimation(fftw_complex* in, int nrf, double *x, double tinc, bool reinit);

		/**
		 * @brief
		 *
		 * @param in
		 * @param powoutput
		 * @param phaseoutput
		 */
		void compute_power_and_phase(fftw_complex* in, double* powoutput, double* phaseoutput);
		/**
		 * @brief
		 *
		 * @param in
		 * @param old_rms
		 * @return int
		 */
		int update_noise(fftw_complex* in, double* old_rms);
		/**
		 * @brief
		 *
		 * @param in
		 * @param powoutput
		 */
		void compute_power(fftw_complex *in, double *powoutput);

		uint fftSize; /**< TODO */
		uint spectrumSize; /**< TODO */
		uint samplingRate; /**< TODO */
};



#endif // NOISE_ESTIMATION_H
