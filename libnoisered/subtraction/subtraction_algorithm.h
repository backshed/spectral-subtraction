#pragma once
#include <complex>

class SubtractionManager;

/**
 * @brief The Subtraction class
 *
 *  Base class from which subtraction algorithms must inherit.
 */
class Subtraction
{
	public:

		Subtraction(SubtractionManager& configuration);
		virtual ~Subtraction();
		/**
		 * @brief Functor : performs the subtraction algorithm.
		 * @param input_spectrum Input spectrum to subtract
		 * @param noise_spectrum Estimated noise spectrum for this frame.
		 */
		virtual void operator()(std::complex<double>* input_spectrum, double* noise_spectrum) = 0;

		/**
		 * @brief Actions to perform if the FFT size changes.
		 *
		 * Most of the buffers will have to change.
		 */
		virtual void onFFTSizeUpdate() = 0;

		/**
		 * @brief Actions to perform if the data changes a lot.
		 *
		 * For instance, discard previous data saved by the algorithms which would not have sense anymore.
		 */
		virtual void onDataUpdate() = 0;

	protected:
		SubtractionManager& conf;
};
