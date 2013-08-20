#include <cmath>

#include "simple_ss.h"
#include "mathutils/math_util.h"
#include "subtraction_manager.h"

SimpleSpectralSubtraction::SimpleSpectralSubtraction(SubtractionManager& configuration):
	Subtraction(configuration)
{
}

SimpleSpectralSubtraction::~SimpleSpectralSubtraction()
{

}

void SimpleSpectralSubtraction::operator()(fftw_complex *input_spectrum, double* noise_spectrum)
{
#pragma omp parallel for
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		double Apower, Bpower, magnitude, phase, power;

		power = MathUtil::CplxToPower(input_spectrum[i]);
		phase = MathUtil::CplxToPhase(input_spectrum[i]);

		Apower = power - _alpha * noise_spectrum[i];
		Bpower = _beta * power;

		magnitude = std::sqrt(std::max(Apower, Bpower));

		input_spectrum[i][0] = magnitude * std::cos(phase);
		input_spectrum[i][1] = magnitude * std::sin(phase);
	}
}

void SimpleSpectralSubtraction::onFFTSizeUpdate()
{

}

void SimpleSpectralSubtraction::onDataUpdate()
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
