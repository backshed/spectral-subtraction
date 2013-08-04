#ifndef MATHUTIL_H
#define MATHUTIL_H

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
	void compute_power_and_phase(fftw_complex *in, double *powoutput, double *phaseoutput, unsigned int size);


	/**
	 * @brief Computes power array from the magnitude spectrum.
	 *
	 * @param in Input spectrum.
	 * @param powoutput Power output.
	 * @param size Size of array.
	 */
	void compute_power(fftw_complex *in, double *powoutput, unsigned int size);

}

#endif // MATHUTIL_H
