#include "wavelet_estimation.h"
#include "simple_estimation.h"
#include "../subtraction/subtraction_algorithm.h"
#include <algorithm>

#include <iostream>
#include "subtraction_manager.h"

WaveletEstimation::WaveletEstimation(SubtractionManager &configuration):
	Estimation(configuration)
{
	algorithm = Algorithm::Simple;
	cwt_noise_estimator.initialize(conf);
}

WaveletEstimation::~WaveletEstimation()
{
	delete[] noise_power_reest;
	fftw_free(tmp_out);
	fftw_free(tmp_spectrum);
}

bool WaveletEstimation::operator()(fftw_complex *input_spectrum)
{
	bool reinit = true; //TODO CAREFUL GUY
	static bool computeMax = false;
	if (reinit) computeMax = false;
	SimpleEstimation simpleEstimation(conf);

	bool reestimated = simpleEstimation(input_spectrum);

	// 1° copy noise_power into noise_power_reest
	std::copy_n(noise_power, conf.spectrumSize(), noise_power_reest);

	if (!reestimated)
	{
		std::copy_n(input_spectrum, conf.spectrumSize(), tmp_spectrum);

		(*conf.getSubtractionImplementation())(input_spectrum, noise_power_reest);

		fftw_execute(plan_bw_temp);

		// 3° Compute CWT and reestimate noise
		cwt_noise_estimator.estimate(tmp_out, noise_power_reest, computeMax);
		computeMax = false;
	}
	else
	{
		// Compute the max. of the frame
		computeMax = true;
	}

	return reestimated;
}

// prepare: quand on change de fftsize par exemple
void WaveletEstimation::onFFTSizeUpdate()
{
	Estimation::onFFTSizeUpdate();

	noise_power_reest = new double[conf.FFTSize()];

	tmp_out = fftw_alloc_real(conf.FFTSize());
	tmp_spectrum = fftw_alloc_complex(conf.spectrumSize());
	plan_bw_temp = fftw_plan_dft_c2r_1d(conf.FFTSize(), tmp_spectrum, tmp_out, FFTW_ESTIMATE);

}

double *WaveletEstimation::noisePower()
{
	return noise_power_reest;
}


void WaveletEstimation::onDataUpdate()
{
	Estimation::onDataUpdate();
	cwt_noise_estimator.clean();
}

