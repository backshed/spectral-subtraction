#pragma once
#include "subtraction_algorithm.h"

/**
 * @brief The SimpleSpectralSubtraction class
 *
 * Performs basic spectral subtraction.
 */
class SimpleSpectralSubtraction : public Subtraction
{
	public:
		SimpleSpectralSubtraction(const SubtractionManager& configuration);
		~SimpleSpectralSubtraction();
		virtual Subtraction* clone() override;

		/**
		 * @brief Performs spectral subtraction, simple algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		virtual void operator()(std::complex<double>* const input_spectrum, const double * const noise_spectrum) override;
		virtual void onFFTSizeUpdate() override;
		virtual void onDataUpdate() override;

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
		void setAlpha(const double value);

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
		void setBeta(const double value);

	protected:
		double _alpha =  0.0; /**< TODO */
		double _beta =  0.0; /**< TODO */

};
