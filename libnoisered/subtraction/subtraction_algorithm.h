#ifndef BASE_SUBTRACTION_ALGORITHM_H
#define BASE_SUBTRACTION_ALGORITHM_H

#include <subtractionconfiguration.h>

class SubtractionAlgorithm
{
	public:
		/**
		 * @brief Algorithm used to perform subtraction.
		 *
		 */
		enum class Algorithm { Standard, EqualLoudness, GeometricApproach, Bypass};
		Algorithm algorithm = Algorithm::Bypass; /**< TODO */


		SubtractionAlgorithm(SubtractionConfiguration& configuration);
		virtual ~SubtractionAlgorithm();
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum);
		virtual void onFFTSizeUpdate();
		virtual void onDataUpdate();

	protected:
		SubtractionConfiguration& conf;
};

#endif // BASE_SUBTRACTION_ALGORITHM_H
