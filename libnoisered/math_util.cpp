#include "math_util.h"
#include <cmath>

namespace MathUtil
{
	// This functions computes the power and the phase vector of the in argument.
	void compute_power_and_phase(fftw_complex *in, double *powoutput, double *phaseoutput, unsigned int size)
	{
		for (auto i = 0U; i < size; ++i)
		{
			powoutput[i] = std::pow(in[i][0], 2) + std::pow(in[i][1], 2);
			phaseoutput[i] = std::atan2(in[i][1], in[i][0]);
		}
	}

	// This functions computes the power of a given spectrum.
	void compute_power(fftw_complex *in, double *powoutput, unsigned int size)
	{
		for (auto i = 0U; i < size; ++i)
		{
			powoutput[i] = std::pow(in[i][0], 2) + std::pow(in[i][1], 2); // std::transform ?
		}
	}
}
