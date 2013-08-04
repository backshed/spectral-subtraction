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
	public:

		/**
		 * @brief Executes a given configuration.
		 *
		 * @param config
		 */
		void execute(SubtractionConfiguration &config);


	private:
		/**** General stuf ****/
		/**
		 * @brief Calls the right subtraction algorithm.
		 *
		 * @param config Configuration.
		 * @param reinit Reinitializes some inner static data if set to true.
		 */
		void subtractionHandler(SubtractionConfiguration &config);
		void estimationHandler(SubtractionConfiguration &config);
};



#endif
