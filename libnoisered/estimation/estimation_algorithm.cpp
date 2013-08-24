#include <algorithm>

#include "estimation_algorithm.h"
#include "subtraction_manager.h"

Estimation::Estimation(SubtractionManager &configuration):
	conf(configuration)
{
}

Estimation::Estimation(const Estimation &est):
	conf(est.conf)
{
	noise_power = new double[conf.spectrumSize()];
	std::copy_n(est.noise_power, conf.spectrumSize(), noise_power);
}

const Estimation &Estimation::operator=(const Estimation &est)
{
	delete[] noise_power;
	noise_power = new double[conf.spectrumSize()];
	std::copy_n(est.noise_power, conf.spectrumSize(), noise_power);

	return *this;
}

Estimation::~Estimation()
{
	delete[] noise_power;
}


void Estimation::onFFTSizeUpdate()
{
	delete[] noise_power;
	noise_power = new double[conf.FFTSize()];
	onDataUpdate();

	specific_onFFTSizeUpdate();
}

void Estimation::onDataUpdate()
{
	std::fill_n(noise_power, conf.FFTSize(), 0);

	specific_onDataUpdate();
}

double *Estimation::noisePower()
{
	return noise_power;
}
