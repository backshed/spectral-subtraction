#include <cmath>
#include <numeric>
#include <algorithm>
#include "mathutils/math_util.h"

namespace Eval
{
	double NRR(double *original, double *reduced, unsigned int length)
	{
		return 10.0 * std::log10(MathUtil::energy(original, length) / MathUtil::energy(reduced, length));
	}

	double SDR(double *original, double *reduced, unsigned int length)
	{
		double gamma = MathUtil::abssum(original, length) / MathUtil::abssum(reduced, length);

		double res = MathUtil::mapReduce2_n(original, reduced, length, 0.0,
											[&] (double x, double y) { return std::pow(x - gamma * y, 2);},
											std::plus<double>());

		return 10.0 * std::log10(MathUtil::energy(original, length) / res);
	}
}
