#pragma once
#include <complex>
// Compter nombre d'instances et faire cleanup sur la dernière.
class FFTManager
{
	public:
		FFTManager();
		virtual ~FFTManager();

		double* input();
		double* output();

		/**
		 * @brief forwardFFT
		 *
		 * From time domain to spectral domain.
		 */
		virtual void forward() = 0;

		/**
		 * @brief backwardFFT Performs a backward FFT
		 *
		 * From spectral domain to time domain.
		 */
		virtual void backward() = 0;

		std::complex<double>* spectrum();

		unsigned int spectrumSize();
		unsigned int size();

		virtual void updateSize(const unsigned int);

	protected:
		std::complex<double>* _spectrum = nullptr;
		double *_in = nullptr;
		double *_out = nullptr;
		unsigned int _fftSize = 0;

};
