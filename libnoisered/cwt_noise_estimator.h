#ifndef CWT_NOISE_ESTIMATOR_H
#define CWT_NOISE_ESTIMATOR_H

#include <vector>
#include <QString>
#include <cwtlib>
#include "area.h"
#include "subtractionconfiguration.h"

using namespace cwtlib;
typedef std::function<void (uint, uint)> ArrayValueFilter;
class CWTNoiseEstimator
{
	public:
		CWTNoiseEstimator();
		void estimate(double *signal_in, double *noise_power, bool computeMax);
		void writeSimpleCWT(double *signal_in);
		void initialize(SubtractionConfiguration& config);
		void clean();
	private:
		int getFFTBinFromFrequency(double freq);
		double getFrequencyFromFFTBin(int bin);
		int getWaveletBinFromFFTBin(int fftbin);

		struct areaParams_
		{
				areaParams_();
				int numAreas;
				double mean;
		};
		std::vector<CWTNoiseEstimator::areaParams_>* areaParams;
		inline void clearAreaParams();

		//**** For CWTLib ****//
		Signal *s;
		LinearRangeFunctor* scales;
		WTransform* wt;
		Matrix *arr;
		std::vector<Area*> areas;
		void writeFiles(QString dir, int file_no);
		void computeCWT(double *signal);
		void computeAreas();
		void applyToArr(std::initializer_list<ArrayValueFilter> funs);
		void computeAreasParameters();
		void reestimateNoise(double *noise_power);

		ArrayValueFilter *copyFromWT;
		void createFilterBinsSeparation();

		uint fftSize;
		uint spectrumSize;
		uint samplingRate;

		double getFreq(int pixel);
		unsigned int getFFTBin(int pixel);
};

#endif // CWT_NOISE_ESTIMATOR_H
