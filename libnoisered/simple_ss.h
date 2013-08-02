#ifndef SIMPLE_SS_H
#define SIMPLE_SS_H
#include "subtraction_algorithm.h"

class SimpleSpectralSubtraction : public SubtractionAlgorithm
{
	public:
		SimpleSpectralSubtraction(SubtractionConfiguration& configuration);
		~SimpleSpectralSubtraction();

		/**
		 * @brief Performs spectral subtraction, simple algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum) override;
		virtual void prepare() override;

		/**
		 * @brief Returns alpha.
		 *
		 * @return double alpha.
		 */
		double alpha() const;

		/**
		 * @brief Sets alpha.
		 *
		 * @param value alpha.
		 */
		void setAlpha(double value);

		/**
		 * @brief Returns beta.
		 *
		 * @return double beta.
		 */
		double beta() const;

		/**
		 * @brief Sets beta.
		 *
		 * @param value beta.
		 */
		void setBeta(double value);

	protected:
		double _alpha; /**< TODO */
		double _beta; /**< TODO */

};

#endif // SIMPLE_SS_H
