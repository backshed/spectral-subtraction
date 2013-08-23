#pragma once
#include "subtraction_algorithm.h"
/**
 * @brief The GeometricSpectralSubtraction class
 *
 * Performs a spectral subtraction using the geometric algorithm.
 */
class GeometricSpectralSubtraction : public Subtraction
{
	public:
		GeometricSpectralSubtraction(const SubtractionManager& configuration);
		virtual ~GeometricSpectralSubtraction();

		virtual void operator()(std::complex<double>* const input_spectrum, const double* const noise_spectrum) override;

		virtual void onFFTSizeUpdate() override;
		virtual void onDataUpdate() override;

	private:
		double *prev_gamma = nullptr; /**< TODO */
		double *prev_halfchi = nullptr; /**< TODO */
};
