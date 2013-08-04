#include "estimation_algorithm.h"

EstimationAlgorithm::EstimationAlgorithm(SubtractionConfiguration &configuration):
	conf(configuration)
{
}

EstimationAlgorithm::~EstimationAlgorithm()
{

}

bool EstimationAlgorithm::operator()(fftw_complex *)
{
	return true;
}

void EstimationAlgorithm::onFFTSizeUpdate()
{
	noise_power = new double[conf.FFTSize()];
}

void EstimationAlgorithm::onDataUpdate()
{

}

double *EstimationAlgorithm::noisePower()
{
	return noise_power;
}
