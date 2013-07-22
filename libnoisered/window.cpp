#define _USE_MATH_DEFINES
#include <cmath>
#include "window.h"
#include "defines.h"

#ifdef _WIN32
#define M_PI 3.14159265359
#endif

void window(double* smp, size_t size, int flag, int increment)
{
	double mul;
	double cst = 2. * M_PI / (size - 1.);
	switch(flag)
	{
		case 0:
			mul = 0.54; //Hamming
			break;
		case 1:
			mul = 0.5; // Hanning
			break;
		default:
			return;
	}
	double window[size];


	for(unsigned int i = 0; i < size; ++i)
	{
		window[i] = sqrt(mul - (1.0 - mul) * cos(i * cst));
	}
	if(increment != 0)
	{
		double sum = 0;
		for(uint i = 0; i < size; i += increment)
			sum += window[i] * window[i];
		sum = sqrt(sum);
		for(uint i = 0; i < size; ++i)
			window[i] /= sum;
	}

	for(unsigned int i = 0; i < size; ++i)
	{
		smp[i] *= window[i];
	}
}
