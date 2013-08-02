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
#include <fftw3.h>
#include "defines.h"
#include "cwt_noise_estimator.h"

class NoiseEstimator;
class SubtractionConfiguration;

/**
 * @brief Performs spectral subtraction on a SubtractionConfiguration object.
 *
 * This class only implements algorithms, all the data is contained into s_data.
 * However, some arrays are used internally so threading is a no-go currently.
 *
 */
class SpectralSubtractor
{
		friend class NoiseEstimator;

	public:
		/**
		 * @brief Constructor.
		 *
		 */
		SpectralSubtractor();

		/**
		 * @brief Destructor.
		 *
		 */
		~SpectralSubtractor();

		/**
		 * @brief Executes a given configuration.
		 *
		 * @param config
		 */
		void execute(SubtractionConfiguration &config);

		/**
		 * @brief Initializes some inner data needed for processing (should be removed when everything is on s_data).
		 *
		 * In particular, initializes arrays and constants whose size and values are relative to fft size, sampling rate, etc...
		 *
		 * @param config Configuration from which initialization must take place.
		 */
		void initialize(SubtractionConfiguration &config);


		/**
		 * @brief Debug function.
		 *
		 * Outputs the specturm data in spectrumList.txt
		 *
		 * @param config Configuration.
		 */
		void outputNoiseSpectrum(SubtractionConfiguration &config); /**< TODO */


	private:

		/**** Algorithms ****/
		/**
		 * @brief Performs spectral subtraction, standard algorithm.
		 *
		 * @param config Configuration.
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		void subtraction(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power);

		/**
		 * @brief Performs spectral subtraction, equal-loudness algorithm.
		 *
		 * @param config Configuration.
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		void subtraction_el(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power);

		/**
		 * @brief Performs spectral subtraction, geometric algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 * @param firstcall Reinitializes some inner static data if set to true.
		 */
		void geom_algo(SubtractionConfiguration &config, fftw_complex *input_spectrum, double *noise_power);

		/**** General stuf ****/
		/**
		 * @brief Calls the right subtraction algorithm.
		 *
		 * @param config Configuration.
		 * @param reinit Reinitializes some inner static data if set to true.
		 */
		void subtractionHandler(SubtractionConfiguration &config);






		/***/
		NoiseEstimator *estimator = nullptr; /**< TODO */


		uint fftSize; /**< TODO */
		uint spectrumSize; /**< TODO */
		uint samplingRate; /**< TODO */
};



#endif
