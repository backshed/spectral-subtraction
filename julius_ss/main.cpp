extern "C"
{
#include "../julius-4.2.3/julius/submain.h"
}
#include "../libnoisered/subtraction_manager.h"

#include <iostream>
#include <fstream>
#include <string>

extern SubtractionManager* s_data;



int main(int argc, char *argv[])
{
	s_data = new SubtractionManager(512, 16000);
	s_data->readParametersFromFile();

	submain(argc, argv);
}
