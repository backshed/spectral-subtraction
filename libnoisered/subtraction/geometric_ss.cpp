#include "geometric_ss.h"
#include <cmath>
#include <algorithm>

#include "subtraction_manager.h"

GeometricSpectralSubtraction::GeometricSpectralSubtraction(SubtractionManager &configuration):
	Subtraction(configuration)
{
	algorithm = Algorithm::GeometricApproach;
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
	prev_gamma = new double[conf.spectrumSize()];
	prev_halfchi = new double[conf.spectrumSize()];
}


void GeometricSpectralSubtraction::operator ()(fftw_complex* input_spectrum, double* noise_spectrum)
{
	static const double geom_alpha = 0.98, geom_beta = 0.98;
	static const double twentysixdb = pow(10., -26. / 20.);
	static const double thirteendb = pow(10., -20. / 20.);

#pragma omp parallel for
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		double gammai, gamma, chi, h, ymag, xmag;

		// 1) Magnitude spectrum
		ymag = std::sqrt(std::pow(input_spectrum[i][0], 2.0) + std::pow(input_spectrum[i][1], 2.0));

		// 3) compute Gamma
		gammai = std::max(thirteendb, std::pow(ymag, 2.0) / noise_spectrum[i]);

		gamma = geom_beta * prev_gamma[i] + (1.0 - geom_beta) * gammai;
		prev_gamma[i] = gamma;

		// 4) compute Chi
		chi = geom_alpha * prev_halfchi[i] + (1.0 - geom_alpha) * pow(sqrt(gamma) - 1.0, 2.0);
		chi = (twentysixdb > chi) ? twentysixdb : chi;

		// 5) compute gain
		h = std::min(1.0, std::sqrt(
						 (1.0 - std::pow(gamma - chi + 1.0, 2.0) / (4.0 * gamma))
					   / (1.0 - std::pow(gamma - 1.0 - chi, 2.0) / (4.0 * chi))
						 ));

		// 6) compute enhanced magnitude spectrum
		xmag = h * ymag;
		prev_halfchi[i] = std::pow(xmag, 2.0) / noise_spectrum[i];

		// 7) reverse FFT
		double orig_phase = std::atan2(input_spectrum[i][1], input_spectrum[i][0]);

		input_spectrum[i][0] = xmag * std::cos(orig_phase);
		input_spectrum[i][1] = xmag * std::sin(orig_phase);
	}
}
