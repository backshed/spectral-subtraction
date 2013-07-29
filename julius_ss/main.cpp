extern "C"
{
#include "../julius-4.2.3/julius/submain.h"
}
#include "../libnoisered/spectral_subtractor.h"
#include "../libnoisered/subtractionconfiguration.h"

#include <iostream>
#include <fstream>
#include <string>

extern SpectralSubtractor* s;
extern SubtractionConfiguration* s_data;

void setSubtractionParameters()
{
	s_data->setIterations(1);

	s_data->setAlpha(3);
	s_data->setBeta(0.01);

	s_data->setNoiseEstimationAlgorithm(SubtractionConfiguration::NoiseEstimationAlgorithm::Simple);
	s_data->setSpectralSubtractionAlgorithm(SubtractionConfiguration::SpectralSubtractionAlgorithm::Standard);
}


int main(int argc, char *argv[])
{
	s = new SpectralSubtractor();
	s_data = new SubtractionConfiguration(512, 16000);
	s->initialize(*s_data);

	s_data->readParametersFromFile();
	//setSubtractionParameters();
	submain(argc, argv);
}
