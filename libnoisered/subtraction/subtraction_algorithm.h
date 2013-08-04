#ifndef BASE_SUBTRACTION_ALGORITHM_H
#define BASE_SUBTRACTION_ALGORITHM_H

#include <fftw3.h>

class SubtractionManager;


class Subtraction
{
	public:
		/**
		 * @brief Algorithm used to perform subtraction.
		 *
		 */
		enum class Algorithm { Standard, EqualLoudness, GeometricApproach, Bypass};
		Algorithm algorithm = Algorithm::Bypass; /**< TODO */


		Subtraction(SubtractionManager& configuration);
		virtual ~Subtraction();
		virtual void operator()(fftw_complex* input_spectrum, double* noise_spectrum);
		virtual void onFFTSizeUpdate();
		virtual void onDataUpdate();

	protected:
		SubtractionManager& conf;
};

#endif // BASE_SUBTRACTION_ALGORITHM_H
