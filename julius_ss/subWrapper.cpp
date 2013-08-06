#include "subWrapper.h"
#include "alsa.h"

#include "../libnoisered/subtraction_manager.h"
#include <iostream>

SubtractionManager* s_data = nullptr;
AlsaManager* alsam = nullptr;
// Améliorer l'allocation : si on est plus grand, juste augmenter, si on est plus petit, ne rien faire
#ifdef __cplusplus
extern "C"
{
#endif
void computeSS(signed short int* buffer, int len)
{
	std::cout << len << std::endl;
	s_data->readBuffer(buffer, len);
	s_data->execute();
	s_data->writeBuffer(buffer);

	alsam->writeToQueue(buffer, len);
}

void resetSS()
{
	s_data->onDataUpdate();
}

#ifdef __cplusplus
}
#endif
