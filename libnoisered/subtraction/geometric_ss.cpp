#include <cmath>
#include <algorithm>

#include "geometric_ss.h"
#include "mathutils/math_util.h"
#include "subtraction_manager.h"

GeometricSpectralSubtraction::GeometricSpectralSubtraction(SubtractionManager &configuration):
	Subtraction(configuration)
{
}

GeometricSpectralSubtraction::~GeometricSpectralSubtraction()
{
	delete[] prev_gamma;
	delete[] prev_halfchi;
}

void GeometricSpectralSubtraction::onDataUpdate()
{
	std::fill_n(prev_gamma, conf.spectrumSize(), 1);
	std::fill_n(prev_halfchi, conf.spectrumSize(), 1);
}


void GeometricSpectralSubtraction::onFFTSizeUpdate()
{
	delete[] prev_gamma;
	delete[] prev_halfchi;
	prev_gamma = new double[conf.spectrumSize()];
	prev_halfchi = new double[conf.spectrumSize()];

	onDataUpdate();
}


void GeometricSpectralSubtraction::operator ()(std::complex<double>* input_spectrum, double* noise_spectrum)
{
	static const double geom_alpha = 0.98, geom_beta = 0.98;
	static const double twentysixdb = pow(10., -26. / 20.);
	static const double thirteendb = pow(10., -20. / 20.);

	#pragma omp parallel for
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		double gammai, gamma, chi, h, ymagn, xmagn, phase;

		// 1) Magnitude spectrum
		ymagn = std::abs(input_spectrum[i]);
		phase = std::arg(input_spectrum[i]);

		// 3) compute Gamma
		gammai = std::max(thirteendb, std::pow(ymagn, 2.0) / noise_spectrum[i]);

		gamma = geom_beta * prev_gamma[i] + (1.0 - geom_beta) * gammai;
		prev_gamma[i] = gamma;

		// 4) compute Chi
		chi = std::max(twentysixdb, geom_alpha * prev_halfchi[i] + (1.0 - geom_alpha) * pow(sqrt(gamma) - 1.0, 2.0));

		// 5) compute gain
		h = std::min(1.0, std::sqrt(
						 (1.0 - std::pow(gamma - chi + 1.0, 2.0) / (4.0 * gamma))
					   / (1.0 - std::pow(gamma - 1.0 - chi, 2.0) / (4.0 * chi))
						 ));

		// 6) compute enhanced magnitude spectrum
		xmagn = h * ymagn;
		prev_halfchi[i] = std::pow(xmagn, 2.0) / noise_spectrum[i];

		// 7) reverse FFT
		input_spectrum[i] = {xmagn * std::cos(phase), xmagn * std::sin(phase)};
	}
}
