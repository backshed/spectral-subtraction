#include "subtraction_algorithm.h"
#include "subtraction_manager.h"

Subtraction::Subtraction(SubtractionManager &configuration):
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
