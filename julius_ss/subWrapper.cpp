#include "subWrapper.h"
#include "../libnoisered/spectral_subtractor.h"
#include "../libnoisered/subtractionconfiguration.h"
#include <iostream>

SpectralSubtractor* s = nullptr;
SubtractionConfiguration* s_data = nullptr;

// Améliorer l'allocation : si on est plus grand, juste augmenter, si on est plus petit, ne rien faire
#ifdef __cplusplus
extern "C"
{
#endif
	void computeSS(signed short int* buffer, int len)
	{
		s_data->readBuffer(buffer, len);
		s->execute(*s_data);

		s_data->writeBuffer(buffer, len);
	}
#ifdef __cplusplus
}
#endif
