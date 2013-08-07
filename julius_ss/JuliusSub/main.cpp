#include <QCoreApplication>
extern "C"
{
#include "../julius-4.2.3/julius/submain.h"
}
#include "../libnoisered/subtraction_manager.h"

#include "audiomanager.h"
#include <thread>
#include <chrono>
#include <iostream>

SubtractionManager* s_data = nullptr;
AudioManager* am = nullptr;

bool ready = false;

void start_thread(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	s_data = new SubtractionManager(512, 16000);
	#ifdef ENABLE_AUDIO
	am = new AudioManager();
	#endif
	s_data->readParametersFromFile();

	ready = true;

	a.exec();
}


int main(int argc, char *argv[])
{
	std::thread mainThread(&start_thread, argc, argv);
	while(!ready)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return submain(argc, argv);
}
