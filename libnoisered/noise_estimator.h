#ifndef NOISE_ESTIMATION_H
#define NOISE_ESTIMATION_H
#include <fftw3.h>
#include "defines.h"
#include "spectral_subtractor.h"

/**
 * @brief Contains the noise estimation algorithms.
 *
 */
class NoiseEstimator
{
		SpectralSubtractor *parent; /**< TODO */
		CWTNoiseEstimator cwt_noise_estimator; /**< TODO */

	public:
		/**
		 * @brief Constructor.
		 *
		 * @param parent Parent is needed to initialize some inner values.
		 */
		NoiseEstimator(SpectralSubtractor *parent);

		/**
		 * @brief Debug function : writes a wavelet transform to a file.
		 *
		 * @param signal Raw audio signal.
		 */
		void writeSimpleCWT(SubtractionConfiguration &config, double *signal);

		/**
		 * @brief Calls the right noise estimation algorithm.
		 *
		 * @param config Configuration.
		 * @param reinit Set to true when inner parameters need to be reinitialized.
		 */
		void estimationHandler(SubtractionConfiguration &config, bool reinit);

		/**
		 * @brief Initializers some internal data.
		 *
		 * @param config Configuration
		 */
		void initialize(SubtractionConfiguration &config);

		/**
		 * @brief Cleans some internal arrays, used by destructor.
		 *
		 */
		void clean(SubtractionConfiguration &config);

	private:

		/**
		 * @brief Wavelet estimation algorithm.
		 *
		 * @param config Configuration.
		 * @param reinit Set to true when inner parameters need to be reinitialized.
		 * @return bool True if a reestimation was performed.
		 */
		bool waveletEstimation(SubtractionConfiguration &config, bool reinit);

		/**
		 * @brief
		 *
		 * @param spectrum Spectrum of the input signal.
		 * @param noise_power Output array where the noise power will be put.
		 * @param reinit Set to true when inner parameters need to be reinitialized.
		 * @return bool True if a reestimation was performed.
		 */
		bool simpleEstimation(SubtractionConfiguration &config, fftw_complex *spectrum, double *noise_power);

		/**
		 * @brief Performs a noise estimation according to the Martin noise estimation algorithm.
		 *
		 * This is a port of estnoisem.m file from VOICEBOX.
		 * Works best with overlap-add processing, and Geometric Approach algorithm.
		 *
		 * Refs:
		 *    [1] Rainer Martin.
		 *        Noise power spectral density estimation based on optimal smoothing and minimum statistics.
		 *        IEEE Trans. Speech and Audio Processing, 9(5):504-512, July 2001.
		 *    [2] Rainer Martin.
		 *        Bias compensation methods for minimum statistics noise power spectral density estimation
		 *        Signal Processing, 2006, 86, 1215-1229
		 *    [3] Dirk Mauler and Rainer Martin
		 *        Noise power spectral density estimation on highly correlated data
		 *        Proc IWAENC, 2006
		 *
		 * @param in Input spectrum.
		 * @param nrf Spectrum size. (Number of frequency bins).
		 * @param x Array where estimated noise power will be placed.
		 * @param tinc Frame increment.
		 * @param reinit True if need to reinit.
		 */
		void martinEstimation(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power);

		/**
		 * @brief Computes power and phase array from the magnitude spectrum.
		 *
		 * @param in Input spectrum.
		 * @param powoutput Power output.
		 * @param phaseoutput Phase output.
		 */
		void compute_power_and_phase(fftw_complex *in, double *powoutput, double *phaseoutput);

		/**
		 * @brief Function that checks if the noise needs an update for simple algorithms.
		 *
		 * Idea is to compute the RMS level and to update only if the new RMS level is equivalent, inferior, or a
		 * bit superior.
		 *
		 * @param in Input spectrum.
		 * @param old_rms Previous RMS value.
		 * @return bool True if the noise power estimation changed.
		 */
		bool update_noise(SubtractionConfiguration &config, fftw_complex *in);

		/**
		 * @brief Computes power array from the magnitude spectrum.
		 *
		 * @param in Input spectrum.
		 * @param powoutput Power output.
		 */
		void compute_power(fftw_complex *in, double *powoutput);

		uint fftSize; /**< TODO */
		uint spectrumSize; /**< TODO */
		uint samplingRate; /**< TODO */
};



#endif // NOISE_ESTIMATION_H
