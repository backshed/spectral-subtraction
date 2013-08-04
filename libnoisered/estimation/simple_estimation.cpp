#include "simple_estimation.h"
#include "util.h"

#include <cmath>

SimpleEstimation::SimpleEstimation(SubtractionConfiguration &configuration):
	EstimationAlgorithm(configuration)
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
		Util::compute_power(input_spectrum, noise_power, conf.spectrumSize());
		return true;
	}
	return false;
}

void SimpleEstimation::onFFTSizeUpdate()
{
	EstimationAlgorithm::onFFTSizeUpdate();
}

// reinit
void SimpleEstimation::onDataUpdate()
{
	noise_rms = 100000;
}


// Function to check whether there is noise in the signal
bool SimpleEstimation::update_noise(fftw_complex *in)
{
	// We estimate the RMS power and compare it to previous noise power
	double current_rms = 0;
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
		current_rms += pow(in[i][0], 2) + pow(in[i][1], 2); // std::transform ?

	current_rms = sqrt(current_rms / conf.spectrumSize());

	//if we find more or less the same power, it might be noise
	if (current_rms <  noise_rms
			|| (current_rms >= noise_rms && current_rms <= noise_rms * 1.02))
	{
		noise_rms = current_rms;
		return true;
	}
	return false;
}
