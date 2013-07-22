#ifndef FILTERS_H
#define FILTERS_H

#include "fftw3.h"
fftw_complex * create_lowpass(double freq);
void convolve(fftw_complex* in, fftw_complex* filter);
#endif // FILTERS_H
