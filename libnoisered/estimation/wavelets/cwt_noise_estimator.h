#pragma once
#include <functional>
#include <cwtlib>
#include "area.h"

class SubtractionManager;

using namespace cwtlib;
/**
 * @brief Type for functions that are applied on arr.
 *
 * Basically, these functions should only do very simple operations,
 * like void f(unsigned int i, unsigned int j) { arr[i][j] += 1; }.
 * These operations should be applied only on the given coordinates.
 */
typedef std::function<void (MaskedMatrix::size_type, MaskedMatrix::size_type)> ArrayValueFilter;

#define AMIN 0                  /* A min  */
#define ASTP 0.05               /* A step */
#define AMAX 64                 /* A max  */

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

		CWTNoiseEstimator(const CWTNoiseEstimator&other);
		const CWTNoiseEstimator& operator=(const CWTNoiseEstimator&);
		~CWTNoiseEstimator();
		/**
		 * @brief Performs musical tones estimation.
		 *
		 * @param signal_in Input signal
		 * @param noise_power Output noise power.
		 * @param computeMax Set to true if the max has to be computed (on a new frame for instance)
		 */
		void estimate(double *signal_in, double *noise_power, bool computeMax);

		/**
		 * @brief Debug function.
		 *
		 * Writes a CWT result to a file.
		 *
		 * @param signal_in Input signal.
		 */
		void writeSimpleCWT(double *signal_in);

		/**
		 * @brief Initializes some inner data.
		 *
		 * @param config Configuration.
		 */
		void initialize(SubtractionManager &config);



	private:
		/**
		 * @brief Contains parameters used into an algorithm.
		 *
		 */
		struct areaParams_
		{
			areaParams_();
			int numAreas; /**< Number of areas */
			double mean; /**< Mean value of these areas */
		};
		std::vector<CWTNoiseEstimator::areaParams_> areaParams = std::vector<CWTNoiseEstimator::areaParams_>(); /**< TODO */

		/**
		 * @brief Clear area parameters.
		 *
		 */
		inline void clearAreaParams();

		//**** For CWTLib ****//
		Signal s = Signal(); /**< TODO */
		LinearRangeFunctor scales = LinearRangeFunctor(AMIN, ASTP, AMAX); /**< TODO */
		WTransform *wt = nullptr; /**< TODO */
		MaskedMatrix arr = MaskedMatrix(); /**< TODO */
		std::vector<Area> areas = std::vector<Area>(); /**< TODO */

		/**
		 * @brief Debug function.
		 *
		 * Write wavelet transform on disk.
		 *
		 * @param dir Directory on which the files must be written.
		 * @param file_no Current file number. (Genreally the number of the wt)
		 */
		void writeFiles(std::string dir, int file_no);

		/**
		 * @brief Core algorithm that computes the wavelet transform.
		 *
		 * @param signal Input signal.
		 */
		void computeCWT(double *signal);

		/**
		 * @brief Computes the areas of a WT.
		 *
		 */
		void computeAreas();

		/**
		 * @brief Apply a list of functions / lambda expressions to the array.
		 *
		 * @param funs List of functions.
		 */
		void applyToArr(std::initializer_list<ArrayValueFilter> funs);

		/**
		 * @brief Computes the parameters of the areas.
		 *
		 * Fills areaParams.
		 *
		 */
		void computeAreasParameters();

		/**
		 * @brief Reestimates the noise power according to the computed musical tone parameters.
		 *
		 * @param noise_power In-place modified noise power array.
		 */
		void reestimateNoise(double *noise_power);

		ArrayValueFilter *copyFromWT = nullptr; /**< TODO */

		/**
		 * @brief Unused.
		 *
		 */
		void createFilterBinsSeparation();

		unsigned int fftSize = 0; /**< TODO */
		unsigned int spectrumSize = 0; /**< TODO */
		unsigned int samplingRate = 0; /**< TODO */

		/**
		 * @brief Gets frequency from a vertical pixel in the WT.
		 *
		 * @param pixel Input pixel.
		 * @return double Approximate frequency.
		 */
		double getFreq(MaskedMatrix::size_type pixel);

		/**
		 * @brief Gets the FFT bin corresponding to a pixel in the WT.
		 *
		 * @param pixel Vertical position of a pixel in the WT.
		 * @return unsigned int Corresponding FFT bin.
		 */
		unsigned long getFFTBin(MaskedMatrix::size_type pixel);
};
