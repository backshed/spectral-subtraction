#include "filters.h"
#include "defines.h"
#include "window.h"

fftw_complex * create_lowpass(double freq)
{
	// Initialisation of the lowpass filter
	double lowpass_kernel[FFT_SIZE];
	fftw_complex *lowpass_fft = (double (*)[2]) fftw_malloc(sizeof(fftw_complex) * SPECTRUM_SIZE);

	fftw_plan plan_FiToR = fftw_plan_dft_c2r_1d ( FFT_SIZE, lowpass_fft, lowpass_kernel, FFTW_ESTIMATE );
	fftw_plan plan_RToFi = fftw_plan_dft_r2c_1d ( FFT_SIZE, lowpass_kernel, lowpass_fft, FFTW_ESTIMATE );

	for(int i = 0; i < SPECTRUM_SIZE ; ++i)
	{
		if(i < freq * SPECTRUM_SIZE / SAMPLING_RATE)
			lowpass_fft[0][i] = 1;
		else
			lowpass_fft[0][i] = 0;
		lowpass_fft[1][i] = 0;
	}

	fftw_execute(plan_FiToR);
	window(lowpass_kernel, FFT_SIZE, 1);
	for(int i = 0; i < FFT_SIZE; ++i)
	{
		lowpass_kernel[i] /= FFT_SIZE;
	}
	fftw_execute(plan_RToFi);

	return lowpass_fft;
}

//Computes the product of an input by a filter
void convolve(fftw_complex* in, fftw_complex* filter)
{
	fftw_complex filtered;
	for(int j = 0; j < SPECTRUM_SIZE; ++j)
	{
		filtered[0] = in[0][j] * filter[0][j] - in[1][j] * filter[1][j];
		filtered[1] = in[0][j] * filter[1][j] + in[1][j] * filter[0][j];

		in[0][j] = filtered[0];
		in[1][j] = filtered[1];
	}
}
