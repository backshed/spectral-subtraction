#ifndef ESTIMATION_ALGORITHM_H
#define ESTIMATION_ALGORITHM_H

#include <fftw3.h>
class SubtractionManager;

class Estimation
{
	public:
		/**
		 * @brief Algorithm used to estimate noise.
		 *
		 */
		enum class Algorithm { Simple, Wavelets, Martin, Bypass };
		Algorithm algorithm = Algorithm::Bypass; /**< TODO */


		Estimation(SubtractionManager& configuration);
		virtual ~Estimation();
		virtual bool operator()(fftw_complex* input_spectrum);
		virtual void onFFTSizeUpdate();
		virtual void onDataUpdate();
		virtual double* noisePower();

	protected:
		SubtractionManager& conf;
		double* noise_power = nullptr;
};

#endif // ESTIMATION_ALGORITHM_H
