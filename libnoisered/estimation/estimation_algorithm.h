#pragma once
#include <complex>

class SubtractionManager;

/**
 * @brief The Estimation class
 *
 * Base class from which estimation algorithms must inherit.
 */
class Estimation
{
	public:
		Estimation(SubtractionManager& configuration);
		Estimation(const Estimation& est);
		const Estimation &operator=(const Estimation& est);
		virtual Estimation* clone() = 0;
		virtual ~Estimation();
		/**
		 * @brief Executes the estimation algorithm.
		 * @param input_spectrum Input from which the algorithm estimates
		 * @return True if a reestimation was performed
		 */
		virtual bool operator()(std::complex<double>* input_spectrum) = 0;
		/**
		 * @brief Actions to perform if the FFT size changes.
		 *
		 * Most of the buffers will have to change.
		 */
		virtual void onFFTSizeUpdate() final;

		/**
		 * @brief Actions to perform if the data changes a lot.
		 *
		 * For instance, discard previous data saved by the algorithms which would not have sense anymore.
		 * In particular, reset the noise estimation.
		 */
		virtual void onDataUpdate() final;

		/**
		 * @brief noisePower
		 * @return the estimated noise power.
		 */
		virtual double* noisePower();

	protected:
		/**
		 * @brief To reimplement in subsequent classes if there is custom data to change.
		 */
		virtual void specific_onFFTSizeUpdate() = 0;

		/**
		 * @brief To reimplement in subsequent classes if there is custom data to change.
		 */
		virtual void specific_onDataUpdate() = 0;

		SubtractionManager& conf;
		double* noise_power = nullptr;
};
