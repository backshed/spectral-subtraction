#include <algorithm>

#include "wavelet_estimation.h"
#include "simple_estimation.h"
#include "../subtraction/subtraction_algorithm.h"
#include "subtraction_manager.h"



WaveletEstimation::WaveletEstimation(SubtractionManager &configuration):
	Estimation(configuration)
{
	cwt_noise_estimator.initialize(conf);
}

WaveletEstimation::WaveletEstimation(const WaveletEstimation &we):
	Estimation(we.conf)
{
	onFFTSizeUpdate();
	std::copy_n(we.noise_power_reest, conf.spectrumSize(), noise_power_reest); /**< TODO */
	std::copy_n(we.tmp_spectrum, conf.spectrumSize(), tmp_spectrum);
	std::copy_n(we.tmp_out, conf.FFTSize(), tmp_out);
}

const WaveletEstimation &WaveletEstimation::operator=(const WaveletEstimation &we)
{
	onFFTSizeUpdate();
	std::copy_n(we.noise_power_reest, conf.spectrumSize(), noise_power_reest); /**< TODO */
	std::copy_n(we.tmp_spectrum, conf.spectrumSize(), tmp_spectrum);
	std::copy_n(we.tmp_out, conf.FFTSize(), tmp_out);

	return *this;
}

WaveletEstimation::~WaveletEstimation()
{
	delete[] noise_power_reest;
	fftw_free(tmp_out);
	fftw_free(tmp_spectrum);
	fftw_destroy_plan(plan_bw_temp);
}

Estimation *WaveletEstimation::clone()
{
	return new WaveletEstimation(*this);
}

bool WaveletEstimation::operator()(std::complex<double> *input_spectrum)
{
	bool reinit = true; //TODO be CAREFUL
	static bool computeMax = false;
	if (reinit) computeMax = false;
	SimpleEstimation simpleEstimation(conf); // Make local ?
	simpleEstimation.onFFTSizeUpdate();

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
void WaveletEstimation::specific_onFFTSizeUpdate()
{
	delete[] noise_power_reest;
	if(tmp_out) fftw_free(tmp_out);
	if(tmp_spectrum) fftw_free(tmp_spectrum);
	if(plan_bw_temp) fftw_destroy_plan(plan_bw_temp);

	noise_power_reest = new double[conf.FFTSize()];

	tmp_out = fftw_alloc_real(conf.FFTSize());
	tmp_spectrum = reinterpret_cast<std::complex<double>*>(fftw_alloc_complex(conf.spectrumSize()));
	plan_bw_temp = fftw_plan_dft_c2r_1d(conf.FFTSize(), reinterpret_cast<fftw_complex*>(tmp_spectrum), tmp_out, FFTW_ESTIMATE);
}

double *WaveletEstimation::noisePower()
{
	return noise_power_reest;
}


void WaveletEstimation::specific_onDataUpdate()
{

}

