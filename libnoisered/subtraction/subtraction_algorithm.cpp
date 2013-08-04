#include "subtraction_algorithm.h"
#include <subtractionconfiguration.h>

Subtraction::Subtraction(SubtractionConfiguration &configuration):
	conf(configuration)
{
}

Subtraction::~Subtraction()
{

}

void Subtraction::operator()(fftw_complex *, double *)
{

}

void Subtraction::onFFTSizeUpdate()
{

}

void Subtraction::onDataUpdate()
{

}
