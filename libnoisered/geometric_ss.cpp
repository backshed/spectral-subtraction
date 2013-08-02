#include "geometric_ss.h"
#include <cmath>
#include <algorithm>

GeometricSpectralSubtraction::GeometricSpectralSubtraction(SubtractionConfiguration &configuration):
	SubtractionAlgorithm(configuration)
{
	algorithm = Algorithm::GeometricApproach;
}

GeometricSpectralSubtraction::~GeometricSpectralSubtraction()
{
	delete[] prev_gamma;
	delete[] prev_halfchi;
}

void GeometricSpectralSubtraction::initializeAlgorithmData()
{
	std::fill_n(prev_gamma, conf.spectrumSize(), 1);
	std::fill_n(prev_halfchi, conf.spectrumSize(), 1);
}


void GeometricSpectralSubtraction::prepare()
{
	prev_gamma = new double[conf.spectrumSize()];
	prev_halfchi = new double[conf.spectrumSize()];
}


void GeometricSpectralSubtraction::operator ()(fftw_complex* input_spectrum, double* noise_spectrum)
{
	static const double geom_alpha = 0.98, geom_beta = 0.98;
	static const double twentysixdb = pow(10., -26. / 20.);
	static const double thirteendb = pow(10., -20. / 20.);

	double gammai, gamma, chi, h, ymag, xmag;

	for (auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		// 1) Magnitude spectrum
		ymag = std::sqrt(std::pow(input_spectrum[i][0], 2.0) + std::pow(input_spectrum[i][1], 2.0));

		// 3) compute Gamma
		gammai = std::pow(ymag, 2.0) / noise_spectrum[i]; //todo max(thirdeendb, expr)
		gammai = (thirteendb > gammai) ? thirteendb : gammai;

		gamma = geom_beta * prev_gamma[i] + (1.0 - geom_beta) * gammai;
		prev_gamma[i] = gamma;

		// 4) compute Chi
		chi = geom_alpha * prev_halfchi[i] + (1.0 - geom_alpha) * pow(sqrt(gamma) - 1.0, 2.0);
		chi = (twentysixdb > chi) ? twentysixdb : chi;

		// 5) compute gain
		h = std::sqrt((1.0 - std::pow(gamma - chi + 1.0, 2.0) / (4.0 * gamma)) / (1.0 - std::pow(gamma - 1.0 - chi, 2.0) / (4.0 * chi)));
		h = (h > 1.0) ? 1.0 : h;

		// 6) compute enhanced magnitude spectrum
		xmag = h * ymag;
		prev_halfchi[i] = std::pow(xmag, 2.0) / noise_spectrum[i];

		// 7) reverse FFT
		double orig_phase = std::atan2(input_spectrum[i][1], input_spectrum[i][0]);
		double orig_amp = xmag;
		input_spectrum[i][0] = orig_amp * std::cos(orig_phase);
		input_spectrum[i][1] = orig_amp * std::sin(orig_phase);
	}
}
