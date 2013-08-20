#pragma once
#include <fftw3.h>

#include "estimation_algorithm.h"
#include "wavelets/cwt_noise_estimator.h"
/**
 * @brief The WaveletEstimation class
 *
 * Wavelet estimation algorithm.
 */
class WaveletEstimation : public Estimation
{
	public:
		WaveletEstimation(SubtractionManager& configuration);
		virtual ~WaveletEstimation();
		virtual bool operator()(std::complex<double>* input_spectrum);

		virtual double *noisePower();

	protected:
		virtual void specific_onFFTSizeUpdate();
		virtual void specific_onDataUpdate();

	private:
		double cwt_amin = 0;
		double cwt_astp = 0.05;
		double cwt_amax = 64;

		CWTNoiseEstimator cwt_noise_estimator; /**< TODO */

		double *noise_power_reest = nullptr; /**< TODO */

		fftw_plan plan_bw_temp = nullptr; /**< TODO */
		std::complex<double> *tmp_spectrum = nullptr; /**< TODO */
		double *tmp_out = nullptr; /**< TODO */

};
