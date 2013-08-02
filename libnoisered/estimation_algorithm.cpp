#include "estimation_algorithm.h"

EstimationAlgorithm::EstimationAlgorithm(SubtractionConfiguration &configuration):
	conf(configuration)
{
}

void EstimationAlgorithm::prepare()
{
	noise_power = new double[conf.FFTSize()];
}

double *EstimationAlgorithm::noisePower()
{
	return noise_power;
}
