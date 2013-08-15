#ifndef GEOMETRIC_SS_H
#define GEOMETRIC_SS_H

#include "subtraction_algorithm.h"
/**
 * @brief The GeometricSpectralSubtraction class
 *
 * Performs a spectral subtraction using the geometric algorithm.
 */
class GeometricSpectralSubtraction : public Subtraction
{
	public:
		GeometricSpectralSubtraction(SubtractionManager& configuration);
		virtual ~GeometricSpectralSubtraction();

		/**
		 * @brief Performs spectral subtraction, geometric algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum);

		virtual void onFFTSizeUpdate();
		virtual void onDataUpdate();

	private:
		double *prev_gamma = nullptr; /**< TODO */
		double *prev_halfchi = nullptr; /**< TODO */
};

#endif // GEOMETRIC_SS_H
