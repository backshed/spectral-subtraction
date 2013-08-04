#ifndef ESTIMATION_ALGORITHM_H
#define ESTIMATION_ALGORITHM_H

#include "subtractionconfiguration.h"

class EstimationAlgorithm
{
	public:
		/**
		 * @brief Algorithm used to estimate noise.
		 *
		 */
		enum class Algorithm { Simple, Wavelets, Martin, Bypass };
		Algorithm algorithm = Algorithm::Bypass; /**< TODO */


		EstimationAlgorithm(SubtractionConfiguration& configuration);
		virtual ~EstimationAlgorithm();
		virtual bool operator()(fftw_complex* input_spectrum);
		virtual void onFFTSizeUpdate();
		virtual void onDataUpdate();
		virtual double* noisePower();

	protected:
		SubtractionConfiguration& conf;
		double* noise_power = nullptr;
};

#endif // ESTIMATION_ALGORITHM_H
