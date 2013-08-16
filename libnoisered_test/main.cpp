#include <subtraction_manager.h>
#include <subtraction/algorithms.h>
#include <estimation/algorithms.h>

int main()
{
	short tab[4096] = {0};
	SubtractionManager s_mgr(512, 16000);
	//Test : Simple Estimation
	s_mgr.setEstimationImplementation(std::shared_ptr<Estimation>(new SimpleEstimation(s_mgr)));

	// Test : Simple Subtraction
	SimpleSpectralSubtraction* subtraction = new SimpleSpectralSubtraction(s_mgr);
	subtraction->setAlpha(12);
	subtraction->setBeta(15);
	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(subtraction));
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	// Test : Equal-Loudness Subtraction
	EqualLoudnessSpectralSubtraction* elss = new EqualLoudnessSpectralSubtraction(s_mgr);
	elss->setAlpha(12);
	elss->setAlphawt(11);
	elss->setBeta(12);
	elss->setBetawt(15);
	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(elss));
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	// Test : Geometric Subtraction
	s_mgr.setSubtractionImplementation(std::shared_ptr<Subtraction>(new GeometricSpectralSubtraction(s_mgr)));
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	// Test : FFT size change
	s_mgr.setFftSize(256);
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	// Test : Martin noise estimation
	s_mgr.setEstimationImplementation(std::shared_ptr<Estimation>(new MartinEstimation(s_mgr)));
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	// Test : Wavelet noise estimation
	s_mgr.setEstimationImplementation(std::shared_ptr<Estimation>(new WaveletEstimation(s_mgr)));
	s_mgr.readBuffer(tab, 4096);
	s_mgr.execute();

	return 0;
}

