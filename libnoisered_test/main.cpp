#include <subtraction_manager.h>
#include <subtraction/algorithms.h>
#include <estimation/algorithms.h>

int main()
{
	short tab[4096] = {0};
	SubtractionManager s_mgr(512, 16000);
	SimpleSpectralSubtraction* subtraction = new SimpleSpectralSubtraction(s_mgr);
	subtraction->setAlpha(12);
	subtraction->setBeta(15);
	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(subtraction));
	s_mgr.setEstimationImplementation(std::shared_ptr<Estimation>(new SimpleEstimation(s_mgr)));

	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	EqualLoudnessSpectralSubtraction* elss = new EqualLoudnessSpectralSubtraction(s_mgr);
	elss->setAlpha(12);
	elss->setAlphawt(11);
	elss->setBeta(12);
	elss->setBetawt(15);

	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(elss));
	s_mgr.setFftSize(256);
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(new GeometricSpectralSubtraction(s_mgr)));
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	return 0;
}

