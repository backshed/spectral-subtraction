#include "simple_ss.h"
#include <cmath>
SimpleSpectralSubtraction::SimpleSpectralSubtraction(SubtractionConfiguration& configuration):
	SubtractionAlgorithm(configuration)
{
	algorithm = Algorithm::Standard;
}

SimpleSpectralSubtraction::~SimpleSpectralSubtraction()
{

}

void SimpleSpectralSubtraction::operator()(fftw_complex *input_spectrum, double* noise_spectrum)
{
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		double Apower, Bpower, magnitude, phase, y;

		y = std::pow(input_spectrum[i][0], 2) + std::pow(input_spectrum[i][1], 2);

		Apower = y - _alpha * noise_spectrum[i];
		Bpower = _beta * y;

		magnitude = sqrt((Apower > Bpower) ? Apower : Bpower);
		phase = std::atan2(input_spectrum[i][1], input_spectrum[i][0]);

		input_spectrum[i][0] = magnitude * std::cos(phase);
		input_spectrum[i][1] = magnitude * std::sin(phase);
	}
}

void SimpleSpectralSubtraction::prepare()
{

}

double SimpleSpectralSubtraction::alpha() const
{
	return _alpha;
}

void SimpleSpectralSubtraction::setAlpha(double value)
{
	_alpha = std::max(value, 0.000001);
}

double SimpleSpectralSubtraction::beta() const
{
	return _beta;
}

void SimpleSpectralSubtraction::setBeta(double value)
{
	_beta = std::max(value, 0.000001);
}
