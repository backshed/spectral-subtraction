#pragma once
#include "simple_ss.h"

/**
 * @brief The EqualLoudnessSpectralSubtraction class
 *
 * Performs a spectral subtraction using equal-loudness contour curves.
 */
class EqualLoudnessSpectralSubtraction : public SimpleSpectralSubtraction
{
	public:
		EqualLoudnessSpectralSubtraction(const SubtractionManager& configuration);
		EqualLoudnessSpectralSubtraction(const EqualLoudnessSpectralSubtraction& el);
		const EqualLoudnessSpectralSubtraction& operator=(const EqualLoudnessSpectralSubtraction& el);
		~EqualLoudnessSpectralSubtraction();
		virtual Subtraction* clone() override;

		virtual void operator()(std::complex<double>* const input_spectrum, const double * const noise_spectrum) override;
		virtual void onFFTSizeUpdate() override;
		virtual void onDataUpdate() override;



		/**
		 * @brief Returns weighted alpha, for Equal-Loudness algorithm.
		 *
		 * @return double alpha_wt.
		 */
		double alphawt() const;

		/**
		 * @brief Sets weighted alpha, for Equal-Loudness algorithm.
		 *
		 * @param value alpha_wt.
		 */
		void setAlphawt(const double value);

		/**
		 * @brief Returns weighted beta, for Equal-Loudness algorithm.
		 *
		 * @return double beta_wt.
		 */
		double betawt() const;

		/**
		 * @brief Sets weighted beta, for Equal-Loudness algorithm.
		 *
		 * @param value beta_wt.
		 */
		void setBetawt(const double value);


	private:
		/**
		 * @brief Loads loudness contour data and interpolates it.
		 *
		 * @param config Configuration.
		 */
		void loadLoudnessContour();
		double *loudness_contour = nullptr; /**< TODO */

		double _alphawt = 0; /**< TODO */
		double _betawt = 0; /**< TODO */
};
