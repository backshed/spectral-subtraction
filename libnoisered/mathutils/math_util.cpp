#include <cmath>
#include <algorithm>
#include <functional>
#include "math_util.h"

namespace MathUtil
{
	double CplxToPower(std::complex<double> val)
	{
		return std::norm(val);
	}

	double CplxToPhase(std::complex<double> val)
	{
		return std::arg(val);
	}

	double energy(double *tab, unsigned int length)
	{
		return mapReduce_n(tab, length, 0.0, [] (double x) { return std::pow(x, 2);}, std::plus<double>());
	}

	double abssum(double *tab, unsigned int length)
	{
		return mapReduce_n(tab, length, 0.0, [] (double x) { return std::abs(x); },  std::plus<double>());
	}

	void computePowerAndPhaseSpectrum(std::complex<double> *in, double *powoutput, double *phaseoutput, unsigned int size)
	{
		std::transform(in, in + size, powoutput, CplxToPower);
		std::transform(in, in + size, phaseoutput, CplxToPhase);
	}

	void computePowerSpectrum(std::complex<double> *in, double *powoutput, unsigned int size)
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
