#include <cmath>

double energy(double* tab, int length)
{
    double ret = 0;
    for(int i = 0; i < length; ++i)
    {
        ret += tab[i] * tab[i];
    }
    return ret;
}

double abssum(double* tab, int length)
{
    double ret = 0;
    for(int i = 0; i < length; ++i)
    {
        ret += std::abs(tab[i]);
    }
    return ret;
}

double NRR(double* original, double* reduced, int length)
{
    return 10 * log10(energy(original, length) / energy(reduced, length));
}

double SDR(double* original, double* reduced, int length)
{
    double gamma = abssum(original, length) / abssum(reduced, length);

    double res = 0;
    for (int i = 0; i < length; ++i)
    {
        res += pow(original[i] - gamma * reduced[i], 2);
    }

    return 10 * log10(energy(original, length) / res);
}

