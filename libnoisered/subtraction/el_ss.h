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
		EqualLoudnessSpectralSubtraction(SubtractionManager& configuration);
		~EqualLoudnessSpectralSubtraction();

		/**
		 * @brief Performs spectral subtraction, equal-loudness algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum) override;
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
		void setAlphawt(double value);

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
		void setBetawt(double value);


	private:
		/**
		 * @brief Loads loudness contour data and interpolates it.
		 *
		 * @param config Configuration.
		 */
		void loadLoudnessContour();
		double *loudness_contour = nullptr; /**< TODO */


		double _alphawt; /**< TODO */
		double _betawt; /**< TODO */


};
