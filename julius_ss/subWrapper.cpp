#include "subWrapper.h"

#include "../libnoisered/subtraction_manager.h"
#include <iostream>

SubtractionManager* s_data = nullptr;

// Améliorer l'allocation : si on est plus grand, juste augmenter, si on est plus petit, ne rien faire
#ifdef __cplusplus
extern "C"
{
#endif
	void computeSS(signed short int* buffer, int len)
	{
		s_data->readBuffer(buffer, len);
		s_data->execute();
		s_data->writeBuffer(buffer);
	}
#ifdef __cplusplus
}
#endif
