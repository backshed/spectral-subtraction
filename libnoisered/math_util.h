#ifndef UTIL_H
#define UTIL_H

#include <fftw3.h>
namespace MathUtil
{
	/**
	 * @brief Computes power and phase array from the magnitude spectrum.
	 *
	 * @param in Input spectrum.
	 * @param powoutput Power output.
	 * @param phaseoutput Phase output.
	 * @param size Size of array.
	 */
	void compute_power_and_phase(fftw_complex *in, double *powoutput, double *phaseoutput, int size);


	/**
	 * @brief Computes power array from the magnitude spectrum.
	 *
	 * @param in Input spectrum.
	 * @param powoutput Power output.
	 * @param size Size of array.
	 */
	void compute_power(fftw_complex *in, double *powoutput, int size);

}

#endif // UTIL_H
