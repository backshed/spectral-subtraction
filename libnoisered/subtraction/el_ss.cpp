#include <cmath>
#include <fstream>
#include <clocale>

#include "el_ss.h"
#include "mathutils/math_util.h"
#include "mathutils/spline.hpp"
#include "subtraction_manager.h"

EqualLoudnessSpectralSubtraction::EqualLoudnessSpectralSubtraction(SubtractionManager &configuration):
	SimpleSpectralSubtraction(configuration)
{
}


void EqualLoudnessSpectralSubtraction::operator()(std::complex<double> *input_spectrum, double *noise_spectrum)
{
	#pragma omp parallel for
	for (auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		double Apower, Bpower, magnitude, phase, power, alpha, beta;
		alpha = _alpha - _alphawt * (loudness_contour[i] - 60);
		beta  = _beta  - _betawt  * (loudness_contour[i] - 60);

		power = std::norm(input_spectrum[i]);
		phase = std::arg(input_spectrum[i]);

		Apower = power - alpha * noise_spectrum[i];
		Bpower = beta * power;

		magnitude = std::sqrt(std::max(Apower, Bpower));

		input_spectrum[i] = {magnitude * std::cos(phase), magnitude * std::sin(phase)};
	}
}

void EqualLoudnessSpectralSubtraction::onFFTSizeUpdate()
{
	loadLoudnessContour();
}

EqualLoudnessSpectralSubtraction::~EqualLoudnessSpectralSubtraction()
{
	delete[] loudness_contour;
}

void EqualLoudnessSpectralSubtraction::loadLoudnessContour()
{
	// loading data for loudness contour algo
#ifdef __linux__
	setlocale(LC_ALL, "POSIX");
	// Because on french OS linux will try to read numbers with commas instead of dots
#endif

	std::ifstream ldata("60phon/loudness_real.data");
	MathUtil::Spline spline;
	double freq, val;
	while(ldata >> freq >> val)
	{
		spline.addPoint(freq, val);
	}
	ldata.close();

	delete[] loudness_contour;
	loudness_contour = new double[conf.spectrumSize()];

	const double freq_bin_span = double(conf.getSamplingRate()) / conf.FFTSize();

	for(auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		loudness_contour[i] = spline(i * freq_bin_span);
	}
}

void EqualLoudnessSpectralSubtraction::onDataUpdate()
{

}


double EqualLoudnessSpectralSubtraction::alphawt() const
{
	return _alphawt;
}

void EqualLoudnessSpectralSubtraction::setAlphawt(double value)
{
	_alphawt = std::max(value, 0.000001);
}
double EqualLoudnessSpectralSubtraction::betawt() const
{
	return _betawt;
}

void EqualLoudnessSpectralSubtraction::setBetawt(double value)
{
	_betawt = std::max(value, 0.000001);
}
