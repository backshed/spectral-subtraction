#ifndef WAVELET_ESTIMATION_H
#define WAVELET_ESTIMATION_H
#include "estimation_algorithm.h"

/**
 * @brief The WaveletEstimation class
 *
 * Wavelet estimation algorithm.
 */
#include <estimation/wavelets/cwt_noise_estimator.h>
class WaveletEstimation : public EstimationAlgorithm
{
	public:
		WaveletEstimation(SubtractionConfiguration& configuration);
		virtual ~WaveletEstimation();
		virtual bool operator()(fftw_complex* input_spectrum);
		virtual void onFFTSizeUpdate();
		virtual void onDataUpdate();
		virtual double *noisePower();

	private:
		double cwt_amin = 0;
		double cwt_astp = 0.05;
		double cwt_amax = 64;

		CWTNoiseEstimator cwt_noise_estimator; /**< TODO */


		double *noise_power_reest = nullptr; /**< TODO */


		fftw_plan plan_bw_temp; /**< TODO */
		fftw_complex *tmp_spectrum = nullptr; /**< TODO */
		double *tmp_out = nullptr; /**< TODO */

};

#endif // WAVELET_ESTIMATION_H
