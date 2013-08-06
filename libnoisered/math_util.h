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

	/**
	 * @brief Puts a signed 16bit integer (red book) between the -1 / 1 range in double.
	 *
	 * @param x Integer to convert.
	 * @return double Corresponding floating point value.
	 */
	double ShortToDouble(short x);
	/**
	 * @brief Puts a double between -1 and 1 into a 16 bit signed integer (red book).
	 *
	 * @param x Double to convert.
	 * @return short Corresponding short value.
	 */
	short DoubleToShort(double x);

}

#endif // MATHUTIL_H
