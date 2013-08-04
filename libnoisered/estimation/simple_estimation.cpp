#include "simple_estimation.h"
#include "../math_util.h"

#include <cmath>
#include "subtraction_manager.h"

SimpleEstimation::SimpleEstimation(SubtractionManager &configuration):
	Estimation(configuration)
{
	algorithm = Algorithm::Simple;
}

SimpleEstimation::~SimpleEstimation()
{

}

bool SimpleEstimation::operator()(fftw_complex *input_spectrum)
{
	if (update_noise(input_spectrum))
	{
		MathUtil::compute_power(input_spectrum, noise_power, conf.spectrumSize());
		return true;
	}
	return false;
}

void SimpleEstimation::onFFTSizeUpdate()
{
	Estimation::onFFTSizeUpdate();
}

// reinit
void SimpleEstimation::onDataUpdate()
{
	Estimation::onDataUpdate();
	noise_rms = 100000;
}


// Function to check whether there is noise in the signal
bool SimpleEstimation::update_noise(fftw_complex *in)
{
	// We estimate the RMS power and compare it to previous noise power
	double current_rms = 0;
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
		current_rms += std::pow(in[i][0], 2) + std::pow(in[i][1], 2); // std::transform ?

	current_rms = std::sqrt(current_rms / conf.spectrumSize());

	//if we find more or less the same power, it might be noise
	if (current_rms <  noise_rms
			|| (current_rms >= noise_rms && current_rms <= noise_rms * 1.02))
	{
		noise_rms = current_rms;
		return true;
	}
	return false;
}
