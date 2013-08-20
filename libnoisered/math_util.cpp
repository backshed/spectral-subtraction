#include "math_util.h"
#include <cmath>
#include <algorithm>

namespace MathUtil
{
	double CplxToPower(fftw_complex val)
	{
		return std::pow(val[0], 2.0) + std::pow(val[1], 2.0);
	}

	double CplxToPhase(fftw_complex val)
	{
		return std::atan2(val[1], val[0]);
	}


	double energy(double *tab, unsigned int length)
	{
		return mapReduce_n(tab, length, 0.0, [] (double x) { return std::pow(x, 2);}, std::plus<double>());
	}

	double abssum(double *tab, unsigned int length)
	{
		return mapReduce_n(tab, length, 0.0, [] (double x) { return std::abs(x); },  std::plus<double>());
	}

	void computePowerAndPhaseSpectrum(fftw_complex *in, double *powoutput, double *phaseoutput, unsigned int size)
	{
		std::transform(in, in + size, powoutput, CplxToPower);
		std::transform(in, in + size, phaseoutput, CplxToPhase);
	}

	void computePowerSpectrum(fftw_complex *in, double *powoutput, unsigned int size)
	{
		std::transform(in, in + size, powoutput, CplxToPower);
	}

	double ShortToDouble(short x)
	{
		const double normalizationFactor = 1.0 / pow(2.0, sizeof(short) * 8 - 1.0);
		return x * normalizationFactor;
	}

	short DoubleToShort(double x)
	{
		const double denormalizationFactor = pow(2.0, sizeof(short) * 8 - 1.0);
		return x * denormalizationFactor;
	}
}
