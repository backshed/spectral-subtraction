#ifndef CWT_NOISE_ESTIMATOR_H
#define CWT_NOISE_ESTIMATOR_H

#include <vector>
#include <cwtlib>
#include "area.h"
#include "subtractionconfiguration.h"

using namespace cwtlib;
/**
 * @brief
 *
 */
typedef std::function<void (uint, uint)> ArrayValueFilter;
/**
 * @brief
 *
 */
class CWTNoiseEstimator
{
	public:
		CWTNoiseEstimator();
		/**
		 * @brief
		 *
		 * @param signal_in
		 * @param noise_power
		 * @param computeMax
		 */
		void estimate(double *signal_in, double *noise_power, bool computeMax);
		/**
		 * @brief
		 *
		 * @param signal_in
		 */
		void writeSimpleCWT(double *signal_in);
		/**
		 * @brief
		 *
		 * @param config
		 */
		void initialize(SubtractionConfiguration& config);
		/**
		 * @brief
		 *
		 */
		void clean();

	private:
		/**
		 * @brief
		 *
		 * @param freq
		 * @return int
		 */
		int getFFTBinFromFrequency(double freq);
		/**
		 * @brief
		 *
		 * @param bin
		 * @return double
		 */
		double getFrequencyFromFFTBin(int bin);
		/**
		 * @brief
		 *
		 * @param fftbin
		 * @return int
		 */
		int getWaveletBinFromFFTBin(int fftbin);

		/**
		 * @brief
		 *
		 */
		struct areaParams_
		{
				areaParams_();
				int numAreas; /**< TODO */ /**< TODO */
				double mean; /**< TODO */
		};
		std::vector<CWTNoiseEstimator::areaParams_>* areaParams; /**< TODO */
		/**
		 * @brief
		 *
		 */
		inline void clearAreaParams();

		//**** For CWTLib ****//
		Signal *s; /**< TODO */
		LinearRangeFunctor* scales; /**< TODO */
		WTransform* wt; /**< TODO */
		Matrix *arr; /**< TODO */
		std::vector<Area*> areas; /**< TODO */
		/**
		 * @brief
		 *
		 * @param dir
		 * @param file_no
		 */
		void writeFiles(std::string dir, int file_no);
		/**
		 * @brief
		 *
		 * @param signal
		 */
		void computeCWT(double *signal);
		/**
		 * @brief
		 *
		 */
		void computeAreas();
		/**
		 * @brief
		 *
		 * @param funs
		 */
		void applyToArr(std::initializer_list<ArrayValueFilter> funs);
		/**
		 * @brief
		 *
		 */
		void computeAreasParameters();
		/**
		 * @brief
		 *
		 * @param noise_power
		 */
		void reestimateNoise(double *noise_power);

		ArrayValueFilter *copyFromWT; /**< TODO */
		/**
		 * @brief
		 *
		 */
		void createFilterBinsSeparation();

		uint fftSize; /**< TODO */
		uint spectrumSize; /**< TODO */
		uint samplingRate; /**< TODO */

		/**
		 * @brief
		 *
		 * @param pixel
		 * @return double
		 */
		double getFreq(int pixel);
		/**
		 * @brief
		 *
		 * @param pixel
		 * @return unsigned int
		 */
		unsigned int getFFTBin(int pixel);
};

#endif // CWT_NOISE_ESTIMATOR_H
