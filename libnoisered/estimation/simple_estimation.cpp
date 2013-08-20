#include "simple_estimation.h"
#include "mathutils/math_util.h"

#include <cmath>
#include <algorithm>
#include "subtraction_manager.h"

SimpleEstimation::SimpleEstimation(SubtractionManager &configuration):
	Estimation(configuration)
{
}

SimpleEstimation::~SimpleEstimation()
{

}

bool SimpleEstimation::operator()(fftw_complex *input_spectrum)
{
	if (updateNoise(input_spectrum))
	{
		MathUtil::computePowerSpectrum(input_spectrum, noise_power, conf.spectrumSize());
		return true;
	}
	return false;
}

void SimpleEstimation::specific_onDataUpdate()
{
	noise_rms = 100000;
}

void SimpleEstimation::specific_onFFTSizeUpdate()
{

}

bool SimpleEstimation::updateNoise(fftw_complex *in)
{
	// We estimate the RMS power and compare it to previous noise power
	double current_rms = std::sqrt(MathUtil::mapReduce_n(in, conf.spectrumSize(), 0.0, MathUtil::CplxToPower, std::plus<double>()) / conf.spectrumSize());

	//if we find more or less the same power, it might be noise
	if (current_rms <  noise_rms ||
	   (current_rms >= noise_rms && current_rms <= noise_rms * 1.02))
	{
		noise_rms = current_rms;
		return true;
	}
	return false;
}
