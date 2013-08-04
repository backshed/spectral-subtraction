#include "math_util.h"
#include <cmath>
#include <algorithm>

namespace MathUtil
{
	static auto ToPower = [&] (fftw_complex val)
	{
		return std::pow(val[0], 2) + std::pow(val[1], 2);
	};
	static auto ToPhase = [&] (fftw_complex val)
	{
		return std::atan2(val[1], val[0]);
	};

	// This functions computes the power and the phase vector of the in argument.
	void compute_power_and_phase(fftw_complex *in, double *powoutput, double *phaseoutput, unsigned int size)
	{
		std::transform(in, in + size, powoutput, ToPower);
		std::transform(in, in + size, phaseoutput, ToPhase);
	}

	// This functions computes the power of a given spectrum.
	void compute_power(fftw_complex *in, double *powoutput, unsigned int size)
	{
		std::transform(in, in + size, powoutput, ToPower);
	}
}
