#include "noise_estimator.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <climits>
#include <functional>
#include <numeric>
#include "subtractionconfiguration.h"

using namespace std;

void NoiseEstimator::initialize(SubtractionConfiguration &config)
{
	samplingRate = config.samplingRate;
	spectrumSize = config.spectrumSize;
	fftSize = config.fftSize;

	cwt_noise_estimator.initialize(config);
}

void NoiseEstimator::clean(SubtractionConfiguration &config)
{
	cwt_noise_estimator.clean(config);
}


// This functions computes the power and the phase vector of the in argument.
void NoiseEstimator::compute_power_and_phase(fftw_complex *in, double *powoutput, double *phaseoutput)
{
	for (auto i = 0U; i < parent->spectrumSize; ++i)
	{
		powoutput[i] = pow(in[i][0], 2) + pow(in[i][1], 2);
		phaseoutput[i] = atan2(in[i][1], in[i][0]);
	}
}

// This functions computes the power of a given spectrum.
void NoiseEstimator::compute_power(fftw_complex *in, double *powoutput)
{
	for (auto i = 0U; i < parent->spectrumSize; ++i)
	{
		powoutput[i] = pow(in[i][0], 2) + pow(in[i][1], 2); // std::transform ?
	}
}

// Function to check whether there is noise in the signal
bool NoiseEstimator::update_noise(SubtractionConfiguration& config, fftw_complex *in)
{
	// We estimate the RMS power and compare it to previous noise power
	double current_rms = 0;
	for (auto i = 0U; i < parent->spectrumSize; ++i)
		current_rms += pow(in[i][0], 2) + pow(in[i][1], 2); // std::transform ?

	current_rms = sqrt(current_rms / parent->spectrumSize);

	//if we find more or less the same power, it might be noise
	if (current_rms <  config.noise_est_cfg.noise_rms
		|| (current_rms >= config.noise_est_cfg.noise_rms && current_rms <= config.noise_est_cfg.noise_rms * 1.02))
	{
		config.noise_est_cfg.noise_rms = current_rms;
		return true;
	}
	return false;
}

bool NoiseEstimator::simpleEstimation(SubtractionConfiguration& config, fftw_complex *spectrum, double *noise_power)
{
	if (update_noise(config, spectrum))
	{
		compute_power(spectrum, noise_power);
		return true;
	}
	return false;
}








//yf : input power spectra
//tinc : frame increment (overlap - add)
//x : output
//reinit : reinit
//segment_number : segment_number
//penser à incrémenter segment_number
void NoiseEstimator::martinEstimation(SubtractionConfiguration& config, fftw_complex *input_spectrum, double *noise_power)
{
	compute_power(input_spectrum, config.noise_est_cfg.yft);
	if(config.noise_est_cfg.segment_number == 0)
	{
		for (auto i = 0U; i < config.spectrumSize; ++i)
		{
			config.noise_est_cfg.p[i] = config.noise_est_cfg.yft[i];
			config.noise_est_cfg.sn2[i] = config.noise_est_cfg.p[i];
			config.noise_est_cfg.pb[i] = config.noise_est_cfg.p[i];
			config.noise_est_cfg.pminu[i] = config.noise_est_cfg.p[i];
			config.noise_est_cfg.pb2[i] = config.noise_est_cfg.p[i] * config.noise_est_cfg.p[i];
			config.noise_est_cfg.lminflag[i] = false;
			config.noise_est_cfg.actmin[i] = INT_MAX;
			config.noise_est_cfg.actminsub[i] = INT_MAX;
		}
	}


	// Main processing
	double acb = 1. / (1. + pow(accumulate(config.noise_est_cfg.p, config.noise_est_cfg.p + config.spectrumSize, 0.) / accumulate(config.noise_est_cfg.yft, config.noise_est_cfg.yft + config.spectrumSize, 0.) - 1., 2));  // alpha_c-bar(t)  (9)
	config.noise_est_cfg.ac = config.noise_est_cfg.aca * config.noise_est_cfg.ac + (1 - config.noise_est_cfg.aca) * max(acb, config.noise_est_cfg.acmax);      // alpha_c(t)  (10)
	for(auto i = 0U; i < config.spectrumSize; ++i)
	{
		config.noise_est_cfg.ah[i] = config.noise_est_cfg.amax * config.noise_est_cfg.ac * 1. / (1. + pow(config.noise_est_cfg.p[i] / config.noise_est_cfg.sn2[i] - 1., 2));    // alpha_hat: smoothing factor per frequency (11)
	}
	double snr = accumulate(config.noise_est_cfg.p, config.noise_est_cfg.p + config.spectrumSize, 0.) / accumulate(config.noise_est_cfg.sn2, config.noise_est_cfg.sn2 + config.spectrumSize, 0.);

	double localmin = min(config.noise_est_cfg.aminh, pow(snr, config.noise_est_cfg.snrexp));
	for(auto i = 0U; i < config.spectrumSize; ++i)
	{
		config.noise_est_cfg.ah[i] = max(config.noise_est_cfg.ah[i], localmin);       // lower limit for alpha_hat (12)
		config.noise_est_cfg.p[i] = config.noise_est_cfg.ah[i] * config.noise_est_cfg.p[i] + (1 - config.noise_est_cfg.ah[i]) * config.noise_est_cfg.yft[i];            // smoothed noisy speech power (3)

		config.noise_est_cfg.b[i] = min(pow(config.noise_est_cfg.ah[i], 2.), config.noise_est_cfg.bmax);              // smoothing constant for estimating periodogram variance (22 + 2 lines)
		config.noise_est_cfg.pb[i] = config.noise_est_cfg.b[i] * config.noise_est_cfg.pb[i] + (1 - config.noise_est_cfg.b[i]) * config.noise_est_cfg.p[i];            // smoothed periodogram (20)
		config.noise_est_cfg.pb2[i] = config.noise_est_cfg.b[i] * config.noise_est_cfg.pb2[i] + (1 - config.noise_est_cfg.b[i]) * pow(config.noise_est_cfg.p[i], 2);     // smoothed periodogram squared (21)

		config.noise_est_cfg.qeqi[i] = max(min((config.noise_est_cfg.pb2[i] - pow(config.noise_est_cfg.pb[i], 2.)) / (2 * pow(config.noise_est_cfg.sn2[i], 2.)), config.noise_est_cfg.qeqimax), config.noise_est_cfg.qeqimin / config.noise_est_cfg.segment_number); // Qeq inverse (23)
	}

	double qiav = accumulate(config.noise_est_cfg.qeqi, config.noise_est_cfg.qeqi + config.spectrumSize, 0) / config.spectrumSize;             // Average over all frequencies (23+12 lines) (ignore non-duplication of DC and nyquist terms)
	double bc = 1. + config.noise_est_cfg.qq.av * sqrt(qiav);             // bias correction factor (23+11 lines)
	for(auto i = 0U; i < config.spectrumSize; ++i)
	{
		config.noise_est_cfg.bmind[i] = 1. + 2. * (config.noise_est_cfg.nd - 1.) * (1. - config.noise_est_cfg.md) / (1. / config.noise_est_cfg.qeqi[i] - 2. * config.noise_est_cfg.md);      // we use the simplified form (17) instead of (15)
		config.noise_est_cfg.bminv[i] = 1. + 2. * (config.noise_est_cfg.nv - 1.) * (1. - config.noise_est_cfg.mv) / (1. / config.noise_est_cfg.qeqi[i] - 2. * config.noise_est_cfg.mv);      // same expression but for sub windows

		config.noise_est_cfg.kmod[i] = bc * config.noise_est_cfg.p[i] * config.noise_est_cfg.bmind[i] < config.noise_est_cfg.actmin[i];        // Frequency mask for new minimum

		if (config.noise_est_cfg.kmod[i])
		{
			config.noise_est_cfg.actmin[i] = bc * config.noise_est_cfg.p[i] * config.noise_est_cfg.bmind[i];
			config.noise_est_cfg.actminsub[i] = bc * config.noise_est_cfg.p[i] * config.noise_est_cfg.bminv[i];
		}
	}


	if (config.noise_est_cfg.subwc > 0 && config.noise_est_cfg.subwc < config.noise_est_cfg.nv)             // middle of buffer - allow a local minimum
	{
		for(auto i = 0U; i < config.spectrumSize; ++i)
		{
			config.noise_est_cfg.lminflag[i] |= config.noise_est_cfg.kmod[i];     // potential local minimum frequency bins
			config.noise_est_cfg.pminu[i] = min(config.noise_est_cfg.actminsub[i], config.noise_est_cfg.pminu[i]);
			config.noise_est_cfg.sn2[i] = config.noise_est_cfg.pminu[i];
		}
	}
	else if (config.noise_est_cfg.subwc >= config.noise_est_cfg.nv)                    // end of buffer - do a buffer switch
	{
		config.noise_est_cfg.ibuf = config.noise_est_cfg.ibuf % config.noise_est_cfg.nu;       // increment actbuf storage pointer
		for(auto i = 0U; i < config.spectrumSize; ++i)
		{
			config.noise_est_cfg.actbuf[config.noise_est_cfg.ibuf][i] = config.noise_est_cfg.actmin[i];        // save sub-window minimum
		}
		// attention, boucle inverse à l'ordre normal de la matrice (on raisonne en "colonnes")
		for(auto i = 0U; i < config.spectrumSize; ++i)
		{
			double tmp = 1;
			for (auto j = 0U; j < config.noise_est_cfg.nu; ++j)
			{
				tmp = min(tmp, config.noise_est_cfg.actbuf[j][i]);
			}
			config.noise_est_cfg.pminu[i] = tmp;
		}

		int tmp_index = -1;
		for(auto i = 0U; i < 4; ++i)
		{
			if (qiav < config.noise_est_cfg.qq.qith[i])
			{
				tmp_index = i;
				break;
			}
		}

		double nsm = config.noise_est_cfg.nsms[tmp_index];           // noise slope max
		for(auto i = 0U; i < config.spectrumSize; ++i)
		{
			config.noise_est_cfg.lmin[i] = config.noise_est_cfg.lminflag[i]
										   && !config.noise_est_cfg.kmod
										   && config.noise_est_cfg.actminsub[i] < nsm * config.noise_est_cfg.pminu[i]
										   && config.noise_est_cfg.actminsub[i] > config.noise_est_cfg.pminu[i];

			if (config.noise_est_cfg.lmin[i])
			{
				config.noise_est_cfg.pminu[i] = config.noise_est_cfg.actminsub[i];
				for (auto j = 0U; j < config.noise_est_cfg.nu; ++j)
				{
					config.noise_est_cfg.actbuf[j][i] = config.noise_est_cfg.pminu[i];
				}
			}

			config.noise_est_cfg.lminflag[i] = 0;
			config.noise_est_cfg.actmin[i] = INT_MAX;
		}
		config.noise_est_cfg.subwc = 0;
	}
	++config.noise_est_cfg.subwc;

	for(auto i = 0U; i < config.spectrumSize; ++i)
	{
		noise_power[i] = config.noise_est_cfg.sn2[i];
		config.noise_est_cfg.qisq[i] = sqrt(config.noise_est_cfg.qeqi[i]);

		// xs[t][i] = sn2[i] * sqrt(0.266 * (nd + 100 * qisq[i]) * qisq[i] / (1. + 0.005 * nd + 6. / nd) / (0.5 * 1 / qeqi[i] + nd - 1));
	}

	config.noise_est_cfg.segment_number++;
}


NoiseEstimator::NoiseEstimator(SpectralSubtractor *parent):
	parent(parent)
{ }

// Reinitialisation : for a file, at each iteration
// For a buffer : at the beginning, and when asked. ie : s_conf.initializeAlgorithmData()
void NoiseEstimator::estimationHandler(SubtractionConfiguration &config, bool reinit)
{
	// noise estimation
	switch (config.estimationAlgo)
	{
		case SubtractionConfiguration::NoiseEstimationAlgorithm::Simple:
			simpleEstimation(config, config.spectrum, config.noise_power);
			break;

		case SubtractionConfiguration::NoiseEstimationAlgorithm::Martin:
			if (config.subtractionAlgo == SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach)
				fftw_execute(config.plan_fw_windowed); // A utiliser dans martinEstimation si GA ?

			if (config.useOLA)
				martinEstimation(config, config.windowed_spectrum, config.noise_power);
			else
				martinEstimation(config, config.windowed_spectrum, config.noise_power);
			break;

		case SubtractionConfiguration::NoiseEstimationAlgorithm::Wavelets:
			waveletEstimation(config, reinit);
			break;

		default:
			throw "critical";
	}
}

bool NoiseEstimator::waveletEstimation(SubtractionConfiguration &config, bool reinit)
{
	static bool computeMax = false;
	if (reinit) computeMax = false;
	bool reestimated = simpleEstimation(config, config.spectrum, config.noise_power);

	// 1° copy noise_power into noise_power_reest
	std::copy_n(config.noise_power, parent->spectrumSize, config.noise_power_reest);

	if (!reestimated)
	{
		std::copy_n(config.spectrum, parent->spectrumSize, config.tmp_spectrum);
		// 2° SS with noise_power_reest
		switch (config.subtractionAlgo)
		{
			case SubtractionConfiguration::SpectralSubtractionAlgorithm::Standard:
				parent->subtraction(config, config.tmp_spectrum, config.noise_power_reest);
				break;
			case SubtractionConfiguration::SpectralSubtractionAlgorithm::EqualLoudness:
				parent->subtraction_el(config, config.tmp_spectrum, config.noise_power_reest);
				break;
			case SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach:
				parent->geom_algo(config, config.tmp_spectrum, config.noise_power_reest);
				break;
			default:
				throw "critical";
		}

		fftw_execute(config.plan_bw_temp);

		// 3° Compute CWT and reestimate noise
		cwt_noise_estimator.estimate(config, config.tmp_out, config.noise_power_reest, computeMax);
		computeMax = false;
	}
	else
	{
		// Compute the max. of the frame
		computeMax = true;
	}

	return reestimated;
}

void NoiseEstimator::writeSimpleCWT(SubtractionConfiguration &config, double *signal)
{
	cwt_noise_estimator.writeSimpleCWT(config, signal);
}
