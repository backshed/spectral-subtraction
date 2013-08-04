#include <cmath>

double energy(double *tab, unsigned int length)
{
	double ret = 0;
	for (auto i = 0U; i < length; ++i)
	{
		ret += tab[i] * tab[i];
	}
	return ret;
}

double abssum(double *tab, unsigned int length)
{
	double ret = 0;
	for (auto i = 0U; i < length; ++i)
	{
		ret += std::abs(tab[i]);
	}
	return ret;
}

double NRR(double *original, double *reduced, unsigned int length)
{
	return 10.0 * log10(energy(original, length) / energy(reduced, length));
}

double SDR(double *original, double *reduced, unsigned int length)
{
	double gamma = abssum(original, length) / abssum(reduced, length);

	double res = 0;
	for (auto i = 0U; i < length; ++i)
	{
		res += pow(original[i] - gamma * reduced[i], 2);
	}

	return 10.0 * log10(energy(original, length) / res);
}

