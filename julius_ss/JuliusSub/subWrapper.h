#ifndef SUBWRAPPER_H
#define SUBWRAPPER_H


#ifdef __cplusplus
extern "C"
{
#endif

void computeSS(signed short int* buffer, int len);
void resetSS();
void startAudioStream();
#ifdef __cplusplus
}
#endif


#endif
