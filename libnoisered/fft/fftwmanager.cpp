#include "fftwmanager.h"


unsigned int FFTWManager::_num_instances = 0;

FFTWManager::FFTWManager()
{
	_num_instances++;
}

FFTWManager::~FFTWManager()
{
	if(_in) fftw_free(_in);
	if(_out) fftw_free(_out);
	if(_spectrum) fftw_free(_spectrum);

	_in = nullptr;
	_out = nullptr;
	_spectrum = nullptr;

	if(plan_fw) fftw_destroy_plan(plan_fw);
	if(plan_bw) fftw_destroy_plan(plan_bw);

	_num_instances--;
	if(_num_instances == 0)
		fftw_cleanup();
}

void FFTWManager::forward()
{
	fftw_execute(plan_fw);
}

void FFTWManager::backward()
{
	fftw_execute(plan_bw);
}

void FFTWManager::updateSize(const unsigned int n)
{
	_fftSize = n;

	if(_in) fftw_free(_in);
	if(_out) fftw_free(_out);
	if(_spectrum) fftw_free(_spectrum);

	_in = fftw_alloc_real(_fftSize);
	_out = fftw_alloc_real(_fftSize);
	_spectrum = reinterpret_cast<std::complex<double>*>(fftw_alloc_complex(spectrumSize()));

	if(plan_fw) fftw_destroy_plan(plan_fw);
	if(plan_bw) fftw_destroy_plan(plan_bw);

	// Initialize the fftw plans
	plan_fw = fftw_plan_dft_r2c_1d(_fftSize, _in, reinterpret_cast<fftw_complex*>(_spectrum), FFTW_ESTIMATE);
	plan_bw = fftw_plan_dft_c2r_1d(_fftSize, reinterpret_cast<fftw_complex*>(_spectrum), _out, FFTW_ESTIMATE);
}

