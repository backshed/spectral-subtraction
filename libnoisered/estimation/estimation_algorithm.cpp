#include "estimation_algorithm.h"
#include <algorithm>

#include "subtractionconfiguration.h"

Estimation::Estimation(SubtractionConfiguration &configuration):
	conf(configuration)
{
}

Estimation::~Estimation()
{

}

bool Estimation::operator()(fftw_complex *)
{
	return true;
}

void Estimation::onFFTSizeUpdate()
{
	noise_power = new double[conf.FFTSize()];
	onDataUpdate();
}

void Estimation::onDataUpdate()
{
	std::fill_n(noise_power, conf.FFTSize(), 0);
}

double *Estimation::noisePower()
{
	return noise_power;
}
