#ifndef EL_SS_H
#define EL_SS_H
#include "simple_ss.h"

class EqualLoudnessSpectralSubtraction : public SimpleSpectralSubtraction
{
	public:
		EqualLoudnessSpectralSubtraction(SubtractionConfiguration& configuration);
		~EqualLoudnessSpectralSubtraction();

		/**
		 * @brief Performs spectral subtraction, equal-loudness algorithm.
		 *
		 * @param input_spectrum Input spectrum.
		 * @param noise_power Estimated noise power.
		 */
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum) override;
		virtual void prepare() override;



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
		 * @brief Loads corresponding loudness contour data, for EL-SS.
		 *
		 * @param config Configuration.
		 */
		void loadLoudnessContour();
		double *loudness_contour = nullptr; /**< TODO */


		double _alphawt; /**< TODO */
		double _betawt; /**< TODO */


};

#endif // EL_SS_H
