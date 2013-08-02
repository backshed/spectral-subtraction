#ifndef GEOMETRIC_SS_H
#define GEOMETRIC_SS_H

#include "subtraction_algorithm.h"
class GeometricSpectralSubtraction : public SubtractionAlgorithm
{
	public:
		GeometricSpectralSubtraction(SubtractionConfiguration& configuration);
		virtual ~GeometricSpectralSubtraction();

		/**
		 * @brief Performs spectral subtraction, geometric algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum);

		virtual void prepare();
		virtual void initializeAlgorithmData();

	private:
		double *prev_gamma = nullptr; /**< TODO */
		double *prev_halfchi = nullptr; /**< TODO */
};

#endif // GEOMETRIC_SS_H
