extern "C"
{
#include "../julius-4.2.3/julius/submain.h"
}
#include "../libnoisered/subtraction_manager.h"

#include "alsa.h"
#include <iostream>
#include <fstream>
#include <string>

extern SubtractionManager* s_data;
extern AlsaManager* alsam;


int main(int argc, char *argv[])
{
	alsam = new AlsaManager();
	s_data = new SubtractionManager(512, 16000);
	s_data->readParametersFromFile();
	alsam->run();

	submain(argc, argv);
}
