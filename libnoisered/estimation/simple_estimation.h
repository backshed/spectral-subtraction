#pragma once
#include "estimation_algorithm.h"

/**
 * @brief The SimpleEstimation class
 *
 * Performs a simple noise estimation, and updates when RMS power is low.
 */
class SimpleEstimation : public Estimation
{
	public:
		SimpleEstimation(SubtractionManager& configuration);
		virtual ~SimpleEstimation();
		virtual bool operator()(std::complex<double>* input_spectrum);

	protected:
		virtual void specific_onDataUpdate();
		virtual void specific_onFFTSizeUpdate();


	private:
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
		bool updateNoise(const std::complex<double> * const in);
		double noise_rms;

};
