#ifndef CWT_NOISE_ESTIMATOR_H
#define CWT_NOISE_ESTIMATOR_H


#include "subtractionconfiguration.h"

using namespace cwtlib;


/**
 * @brief This class performs the proposed musical tone reduction method using wavelet transform.
 *
 */
class CWTNoiseEstimator
{
	public:
		/**
		 * @brief Constructor.
		 */
		CWTNoiseEstimator();

		/**
		 * @brief Performs musical tones estimation.
		 *
		 * @param signal_in Input signal
		 * @param noise_power Output noise power.
		 * @param computeMax Set to true if the max has to be computed (on a new frame for instance)
		 */
		void estimate(SubtractionConfiguration &config, double *signal_in, double *noise_power, bool computeMax);

		/**
		 * @brief Debug function.
		 *
		 * Writes a CWT result to a file.
		 *
		 * @param signal_in Input signal.
		 */
		void writeSimpleCWT(SubtractionConfiguration &config, double *signal_in);

		/**
		 * @brief Initializes some inner data.
		 *
		 * @param config Configuration.
		 */
		void initialize(SubtractionConfiguration &config);

		/**
		 * @brief Cleanup some inner data.
		 *
		 * Called by the destructor.
		 *
		 */
		void clean(SubtractionConfiguration &config);

	private:
		/**
		 * @brief Clear area parameters.
		 *
		 */
		inline void clearAreaParams(SubtractionConfiguration &config);

		/**
		 * @brief Contains parameters used into an algorithm.
		 *
		 */
		//TODO put this in s_data to enable threading.




		//**** For CWTLib ****//


		/**
		 * @brief Debug function.
		 *
		 * Write wavelet transform on disk.
		 *
		 * @param dir Directory on which the files must be written.
		 * @param file_no Current file number. (Genreally the number of the wt)
		 */
		void writeFiles(SubtractionConfiguration &config, std::string dir, int file_no);

		/**
		 * @brief Core algorithm that computes the wavelet transform.
		 *
		 * @param signal Input signal.
		 */
		void computeCWT(SubtractionConfiguration &config, double *signal);

		/**
		 * @brief Computes the areas of a WT.
		 *
		 */
		void computeAreas(SubtractionConfiguration &config);

		/**
		 * @brief Apply a list of functions / lambda expressions to the array.
		 *
		 * @param funs List of functions.
		 */
		void applyToArr(SubtractionConfiguration &config, std::initializer_list<ArrayValueFilter> funs);

		/**
		 * @brief Computes the parameters of the areas.
		 *
		 * Fills areaParams.
		 *
		 */
		void computeAreasParameters(SubtractionConfiguration &config);

		/**
		 * @brief Reestimates the noise power according to the computed musical tone parameters.
		 *
		 * @param noise_power In-place modified noise power array.
		 */
		void reestimateNoise(SubtractionConfiguration &config, double *noise_power);

		/**
		 * @brief Unused.
		 *
		 */
		void createFilterBinsSeparation();





		uint fftSize; /**< TODO */
		uint spectrumSize; /**< TODO */
		uint samplingRate; /**< TODO */

		/**
		 * @brief Gets frequency from a vertical pixel in the WT.
		 *
		 * @param pixel Input pixel.
		 * @return double Approximate frequency.
		 */
		double getFreq(int pixel);

		/**
		 * @brief Gets the FFT bin corresponding to a pixel in the WT.
		 *
		 * @param pixel Vertical position of a pixel in the WT.
		 * @return unsigned int Corresponding FFT bin.
		 */
		unsigned int getFFTBin(int pixel);
};

#endif // CWT_NOISE_ESTIMATOR_H
