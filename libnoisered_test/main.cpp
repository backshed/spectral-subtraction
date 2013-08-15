#include <subtraction_manager.h>
#include <subtraction/algorithms.h>
#include <estimation/algorithms.h>

int main()
{
	SubtractionManager s_mgr(512, 16000);
	SimpleSpectralSubtraction* subtraction = new SimpleSpectralSubtraction(s_mgr);
	subtraction->setAlpha(12);
	subtraction->setBeta(15);
	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(subtraction));
	s_mgr.setEstimationImplementation(std::shared_ptr<Estimation>(new SimpleEstimation(s_mgr)));
	return 0;
}

