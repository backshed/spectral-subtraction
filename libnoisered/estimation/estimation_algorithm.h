#ifndef ESTIMATION_ALGORITHM_H
#define ESTIMATION_ALGORITHM_H

#include <fftw3.h>
class SubtractionManager;

/**
 * @brief The Estimation class
 *
 * Base class from which estimation algorithms must inherit.
 */
class Estimation
{
	public:
		/**
		 * @brief Algorithm used to estimate noise.
		 *
		 */
		enum class Algorithm { Simple, Wavelets, Martin, Bypass };
		/**
		 * @brief algorithm Algorithm used to perform noise estimation. Must be set in subclass constructor.
		 */
		Algorithm algorithm = Algorithm::Bypass; /**< TODO */


		Estimation(SubtractionManager& configuration);
		virtual ~Estimation();
		/**
		 * @brief operator () Executes the estimation algorithm.
		 * @param input_spectrum Input from which the algorithm estimates
		 * @return True if a reestimation was performed
		 */
		virtual bool operator()(fftw_complex* input_spectrum);
		/**
		 * @brief onFFTSizeUpdate Actions to perform if the FFT size changes.
		 *
		 * Most of the buffers will have to change.
		 */
		virtual void onFFTSizeUpdate();

		/**
		 * @brief onDataUpdate Actions to perform if the data changes a lot.
		 *
		 * For instance, discard previous data saved by the algorithms which would not have sense anymore.
		 * In particular, reset the noise estimation.
		 */
		virtual void onDataUpdate();

		/**
		 * @brief noisePower
		 * @return the estimated noise power.
		 */
		virtual double* noisePower();

	protected:
		SubtractionManager& conf;
		double* noise_power = nullptr;
};

#endif // ESTIMATION_ALGORITHM_H
