#include "noise_estimator.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <climits>
#include <functional>
#include <numeric>
#include "subtractionconfiguration.h"
using namespace std;

void NoiseEstimator::initialize(SubtractionConfiguration& config)
{
	samplingRate = config.samplingRate;
	spectrumSize = config.spectrumSize;
	fftSize = config.fftSize;

	cwt_noise_estimator.initialize(config);
}

void NoiseEstimator::clean()
{
	cwt_noise_estimator.clean();
}


// This functions computes the power and the phase vector of the in argument.
void NoiseEstimator::compute_power_and_phase(fftw_complex* in, double* powoutput, double* phaseoutput)
{
	for(unsigned int i = 0; i < parent->spectrumSize; ++i)
	{
		powoutput[i] = pow(in[i][0], 2) + pow(in[i][1], 2);
		phaseoutput[i] = atan2(in[i][1], in[i][0]);
	}
}

// This functions computes the power of a given spectrum.
void NoiseEstimator::compute_power(fftw_complex* in, double* powoutput)
{
	for(unsigned int i = 0; i < parent->spectrumSize; ++i)
	{
		powoutput[i] = in[i][0] * in[i][0] + in[i][1] * in[i][1];
	}
}

// Function to check whether there is noise in the signal
int NoiseEstimator::update_noise(fftw_complex* in, double* old_rms)
{
	// We estimate the RMS power and compare it to previous noise power
	double current_rms = 0;
	for(unsigned int i = 0; i < parent->spectrumSize; ++i)
		current_rms += pow(in[i][0], 2) + pow(in[i][1], 2);

	current_rms = sqrt(current_rms / parent->spectrumSize);

	//if we find more or less the same power, it might be noise
	if (   current_rms <  *old_rms
	   || (current_rms >= *old_rms && current_rms <= *old_rms * 1.02))
	{
		*old_rms = current_rms;
		return 1;
	}
	return 0;
}

bool NoiseEstimator::simpleEstimation(fftw_complex* spectrum, double* noise_power, bool reinit)
{
	static double noise_rms;
	if(reinit)
		noise_rms = 100000;

	if(update_noise(spectrum, &noise_rms))
	{
		compute_power(spectrum, noise_power);
		return true;
	}
	return false;
}


struct NoiseParams
{
		double taca;
		double tamax;
		double taminh;
		double tpfall;
		double tbmax;
		double qeqmin;
		double qeqmax;
		double av;
		double td;
		int nu;
		double qith[4];
		double nsmdb[4];
};


void mh_values(double d, double* m, double* h)
{
	static double dmh[3][18] = {
		{1, 2, 5, 8, 10, 15, 20, 30, 40, 60, 80, 120, 140, 160, 180, 220, 260, 300},
		{0, .26, .48, .58, .61, .668, .705, .762, .8, .841, .865, .89, .9, .91, .92, .93, .935, .94},
		{0, 0.15, 0.48, 0.78, 0.98, 1.55, 2, 2.3, 2.52, 3.1, 3.38, 4.15, 4.35, 4.25, 3.9, 4.1, 4.7, 5}};
	int i;
	int ichosen = -1, jchosen = -1;
	for(i = 0; i < 18; ++i )
	{
		if(dmh[0][i] >= d)
		{
			ichosen = i;
			jchosen = i - 1;
			break;
		}
	}
	if(ichosen == -1)
	{
		ichosen = 17;
		jchosen = 17;
	}

	if ( d == dmh[0][ichosen] )
	{
		*m = dmh[1][ichosen];
		*h = dmh[2][ichosen];
	}
	else
	{
		double qj = sqrt(dmh[0][jchosen]);
		double qi = sqrt(dmh[0][ichosen]);
		double q  = sqrt(d);
		*h = dmh[2][ichosen] + (q - qi) * (dmh[2][jchosen] - dmh[2][ichosen]) / (qj - qi);
		*m = dmh[1][ichosen] + (qi * qj / q - qj) * (dmh[1][jchosen] - dmh[1][ichosen]) / (qi - qj);
	}
}


//yf : input power spectra
//tinc : frame increment (overlap - add)
//x : output
//reinit : reinit
//segment_number : segment_number
//penser à incrémenter segment_number
void NoiseEstimator::martinEstimation(fftw_complex* spectrum, int nrf, double *x, double tinc, bool reinit)
{
	static int subwc;
	static int segment_number;
	static double* yft = 0;
	static double* p = 0;
	static double* sn2 = 0;
	static double* pb = 0;
	static double* pminu = 0;
	static double* pb2 = 0;
	static double* actmin = 0;
	static double* actminsub = 0;

	static double *ah = 0;
	static double *b = 0;
	static double *qeqi = 0;
	static double *bmind = 0;
	static double *bminv = 0;
	static double *lmin = 0;
	static double *qisq = 0;
	static bool *kmod = 0;
	static bool *lminflag = 0;
	static double **actbuf = 0;

	static NoiseParams qq;

	static int nu;
	static int ibuf;
	static double ac;
	static double aca;
	static double acmax;
	static double amax;
	static double aminh;
	static double bmax;
	static double snrexp;
	static double nv, nd;
	static double md, hd, mv, hv;
	static double qeqimax;
	static double qeqimin;
	static double nsms[4];


	// Initialisation
	if(reinit)
	{
		segment_number = 0;
		ibuf = 0;
		qq.taca = 0.0449;    // smoothing time constant for alpha_c = -tinc/log(0.7) in equ (11)
		qq.tamax = 0.392;    // max smoothing time constant in (3) = -tinc/log(0.96)
		qq.taminh = 0.0133;    // min smoothing time constant (upper limit) in (3) = -tinc/log(0.3)
		qq.tpfall = 0.064;   // time constant for P to fall (12)
		qq.tbmax = 0.0717;   // max smoothing time constant in (20) = -tinc/log(0.8)
		qq.qeqmin = 2.;       // minimum value of Qeq (23)
		qq.qeqmax = 14.;      // max value of Qeq per frame
		qq.av = 2.12;             // fudge factor for bc calculation (23 + 13 lines)
		qq.td = 1.536;       // time to take minimum over
		qq.nu = 8;           // number of subwindows
		qq.qith[0] = 0.03;
		qq.qith[1] = 0.05;
		qq.qith[2] = 0.06;
		qq.qith[3] = INT_MAX;
		qq.nsmdb[0] = 47.;
		qq.nsmdb[1] = 31.4;
		qq.nsmdb[2] = 15.7;
		qq.nsmdb[3] = 4.1;

		nu 		= qq.nu;
		ac 		= 1;
		aca 	= exp(-tinc / qq.taca); // smoothing constant for alpha_c in equ (11) = 0.7
		acmax 	= aca;          // min value of alpha_c = 0.7 in equ (11) also = 0.7
		amax	= exp(-tinc / qq.tamax); // max smoothing constant in (3) = 0.96
		aminh	= exp(-tinc / qq.taminh); // min smoothing constant (upper limit) in (3) = 0.3
		bmax	= exp(-tinc / qq.tbmax); // max smoothing constant in (20) = 0.8
		snrexp	= -tinc / qq.tpfall;
		nv		= round(qq.td / (tinc * qq.nu));    // length of each subwindow in frames
		if (nv < 4)
		{            // algorithm doesn't work for miniscule frames
			nv = 4;
			nu = max(round(qq.td / (tinc * nv)), 1.);
		}
		subwc = nv;


		nd = nu * nv;           // length of total window in frames

		mh_values(nd, &md, &hd);
		mh_values(nv, &mv, &hv);

		for(int i = 0; i < 4; ++i)
			nsms[i] = pow(10., qq.nsmdb[i] * nv * tinc / 10.);  // [8 4 2 1.2] in paper

		qeqimax = 1. / qq.qeqmin;  // maximum value of Qeq inverse (23)
		qeqimin = 1. / qq.qeqmax; // minumum value of Qeq per frame inverse

		if(ah) delete ah;
		ah = new double[nrf];
		if(b) delete b;
		b = new double[nrf];
		if(qeqi) delete qeqi;
		qeqi = new double[nrf];
		if(bmind) delete bmind;
		bmind = new double[nrf];
		if(bminv) delete bminv;
		bminv = new double[nrf];
		if(lmin) delete lmin;
		lmin = new double[nrf];
		if(qisq) delete qisq;
		qisq = new double[nrf];
		if(kmod) delete kmod;
		kmod = new bool[nrf];

		if(yft) delete yft;
		yft = new double[nrf];
		if(p) delete p;
		p = new double[nrf];
		if(sn2) delete sn2;
		sn2 = new double[nrf];
		if(pb) delete pb;
		pb = new double[nrf];
		if(pminu) delete pminu;
		pminu = new double[nrf];
		if(pb2) delete pb2;
		pb2 = new double[nrf];
		if(lminflag) delete lminflag;
		lminflag = new bool[nrf];
		if(actmin) delete actmin;
		actmin = new double[nrf];
		if(actminsub) delete actminsub;
		actminsub = new double[nrf];

		if(actbuf)
		{
			for(int i = 0; i < nu; ++i)
			{
				delete actbuf[i];
			}
			delete actbuf;
		}
		actbuf = new double*[nu];
		for(int i = 0; i < nu; ++i)
		{
			actbuf[i] = new double[nrf];
			for(int j = 0; j < nrf; ++j)
			{
				actbuf[i][j] = INT_MAX;
			}
		}

		compute_power(spectrum, yft);
		for(int i = 0; i < nrf; ++i)
		{
			p[i] = yft[i];
			sn2[i] = p[i];
			pb[i] = p[i];
			pminu[i] = p[i];
			pb2[i] = p[i] * p[i];
			lminflag[i] = false;
			actmin[i] = INT_MAX;
			actminsub[i] = INT_MAX;
		}
	}
	else
	{
		compute_power(spectrum, yft);
		segment_number++;
	}



	// Main processing
	double acb = 1. / (1. + pow(accumulate(p, p + nrf, 0.) / accumulate(yft, yft + nrf, 0.) - 1., 2));  // alpha_c-bar(t)  (9)
	ac = aca * ac + (1 - aca) * max(acb,acmax);       // alpha_c(t)  (10)
	for(int i = 0; i < nrf; ++i)
	{
		ah[i] = amax * ac * 1. / (1. + pow(p[i] / sn2[i] - 1., 2));    // alpha_hat: smoothing factor per frequency (11)
	}
	double snr = accumulate(p, p + nrf, 0.) / accumulate(sn2, sn2 + nrf, 0.);

	double localmin = min(aminh, pow(snr, snrexp));
	for(int i = 0; i < nrf; ++i)
	{
		ah[i] = max(ah[i], localmin);       // lower limit for alpha_hat (12)
		p[i] = ah[i] * p[i] + (1 - ah[i]) * yft[i];            // smoothed noisy speech power (3)

		b[i] = min(pow(ah[i], 2.), bmax);              // smoothing constant for estimating periodogram variance (22 + 2 lines)
		pb[i] = b[i] * pb[i] + (1 - b[i]) * p[i];            // smoothed periodogram (20)
		pb2[i] = b[i] * pb2[i] + (1 - b[i]) * pow(p[i], 2);    	// smoothed periodogram squared (21)

		qeqi[i]=max(min((pb2[i]-pow(pb[i], 2.)) / (2*pow(sn2[i], 2.)), qeqimax), qeqimin / segment_number);   // Qeq inverse (23)
	}

	double qiav = accumulate(qeqi, qeqi + nrf, 0) / nrf;             // Average over all frequencies (23+12 lines) (ignore non-duplication of DC and nyquist terms)
	double bc = 1. + qq.av * sqrt(qiav);             // bias correction factor (23+11 lines)
	for(int i = 0; i < nrf; ++i)
	{
		bmind[i] = 1. + 2. * (nd - 1.) * (1. - md) / (1. / qeqi[i] - 2. * md);      // we use the simplified form (17) instead of (15)
		bminv[i] = 1. + 2. * (nv - 1.) * (1. - mv) / (1. / qeqi[i] - 2. * mv);      // same expression but for sub windows

		kmod[i] = bc * p[i] * bmind[i] < actmin[i];        // Frequency mask for new minimum

		if(kmod[i])
		{
			actmin[i] = bc * p[i] * bmind[i];
			actminsub[i] = bc * p[i] * bminv[i];
		}
	}


	if(subwc > 0 && subwc < nv)              // middle of buffer - allow a local minimum
	{
		for(int i = 0; i < nrf; ++i)
		{
			lminflag[i] |= kmod[i];    	// potential local minimum frequency bins
			pminu[i] = min(actminsub[i], pminu[i]);
			sn2[i] = pminu[i];
		}
	}
	else if (subwc >= nv)                    // end of buffer - do a buffer switch
	{
		ibuf = ibuf % nu;     	// increment actbuf storage pointer
		for(int i = 0; i < nrf; ++i)
		{
			actbuf[ibuf][i] = actmin[i];    	// save sub-window minimum
		}
		// attention, boucle inverse à l'ordre normal de la matrice (on raisonne en "colonnes")
		for(int i = 0; i < nrf; ++i)
		{
			double tmp = 1;
			for(int j = 0; j < nu; ++j)
			{
				tmp = min(tmp, actbuf[j][i]);
			}
			pminu[i] = tmp;
		}

		int tmp_index = -1;
		for(int i = 0; i < 4; ++i)
		{
			if(qiav < qq.qith[i])
			{
				tmp_index = i;
				break;
			}
		}

		double nsm = nsms[tmp_index];          	// noise slope max
		for(int i = 0; i < nrf; ++i)
		{
			lmin[i] = lminflag[i] && !kmod && actminsub[i] < nsm * pminu[i] && actminsub[i] > pminu[i];

			if(lmin[i])
			{
				pminu[i] = actminsub[i];
				for(int j = 0; j < nu; ++j)
				{
					actbuf[j][i] = pminu[i];
				}
			}

			lminflag[i] = 0;
			actmin[i] = INT_MAX;
		}
		subwc = 0;
	}
	++subwc;

	for(int i = 0; i < nrf; ++i)
	{
		x[i] = sn2[i];
		qisq[i] = sqrt(qeqi[i]);

		// xs[t][i] = sn2[i] * sqrt(0.266 * (nd + 100 * qisq[i]) * qisq[i] / (1. + 0.005 * nd + 6. / nd) / (0.5 * 1 / qeqi[i] + nd - 1));
	}
}




// on initialise le tableau des P(lambda-1, k) à |Y(lambda, k)|²
// on initialise le tableau des sigma(lambda-1, k)
// nonP(lbd-1)
// nonP²(lbd-1)
// actmin(lambda, k) mis à un max (lequel?)
// Pmin_u(lambda, k) : minimum sur la fenêtre de longueur D
// lmin_flag
// choix de D, M(D), H(D) en fonction de la taille de la fenêtre
// longueur de la FFT : L => U, V. Prendre U = 8, V = 12

//*1
// on calcule  ãc = 1 / (1 + (somme des P(lambda-1) / somme des Y(k)² - 1)²
// on calcule ac = 0.7ac(lambda-1) + 0.3max(ãc, 0.7)
// on calcule â = 0.96 * ac(lambda) / (1 + (P(lmabda-1, k) / sigma²(lambda-1, k) - 1)²)

//*2 (à partir d'ici on note a = â
// on calcule les nouveaux P(lambda, k) = a * P(lambda-1) + (1 - a) * |Y(lambda)|²
// (dans un tableau à part, on remplace dans les l-1 à la fin)

//*3
// on calcule B = max(a², 0.8)
// on calcule nonP =  B * nonP (lbd - 1) + (1 - B) P(k)
// on calcule nonP² = B * nonP²(lbd - 1) + (1 - B) P²(lbd)
// on les stocke dans un tableau chacun
// on calcule la variance de P = nonP² - (nonP)²
// on en déduit Qeq = var{P} / (2 * sigma(lambda-1)^4)  (on fait le max avec 2)

// on calcule  ~Q = (Q - 2M) / (1 - M)
// on calcule aussi nonQ^-1 = 1 / L * somme (1 / Q)
// on calcule bc = 1 + 2.12 sqrt( nonQ^-1 )
// on calcule bmin = 1 + (D-1) * 2 / (~Q)


// on met k_mod(k) = 0
// si P * Bmin * Bc < actmin
//  actmin = P * Bmin * Bc
//  actmin_sub = P * Bmin_sub * Bc
//  k_mod(k) = 1

// si subwc = V
//  si k_mod(k) = 1
//   lmin_flag = 0
//  sauver actmin(lambda, k)
//  trouver Pmin_u = min sur les U dernières valeurs de actmin stockées
//
//  if (nonQ^-1 < 0.03) noise_slop_max = 8
//  elif (nonQ^-1 < 0.05) noise_slop_max = 4
//  elif (nonQ^-1 < 0.06) noise_slop_max = 2
//  else noise_slop_max = 1.2
//
//  if lmin_flag && actmin_sub < noise_slope_max * Pmin_u && actmin_sub > Pminu_u
//   Pmin_u = actmin_sub
//   remplacer toutes les valeurs précédemment stockées de actmin par actmin_sub
//  lmin_flag = 0
//  subwc = 1, actmin = max(actmin..), actmin_sub = max(actmin_sub..)
// sinon
//  si subwc > 1
//   si k_mod(k) == 1
//    lmin_flag(l, k) = 1
//   sigma²(l, k) = min (actmin_sub(l, k), Pmin_u(l, k))
//   Pmin_u = sigma²
// subwc ++

//#define MAX_ACTMIN 30
//void martin_noiseDetect(fftw_complex* in, double* power_out)
//{
//	static bool prev_power_initialized = false;
//	static double P[SPECTRUM_SIZE] = {0};
//	static double ac[SPECTRUM_SIZE] = {0};
//	static double sigma2[SPECTRUM_SIZE] = {0};
//	static double nonP[SPECTRUM_SIZE] = {0};
//	static double Q[SPECTRUM_SIZE] = {0};
//	static double nonP2[SPECTRUM_SIZE] = {0};
//	static double actmin[MAX_ACTMIN][SPECTRUM_SIZE];
//	static double actmin_sub[MAX_ACTMIN][SPECTRUM_SIZE];
//	static double Pmin_u[SPECTRUM_SIZE] = {0};
//	static double k_mod[SPECTRUM_SIZE] = {0};
//	static double lmin_flag[SPECTRUM_SIZE] = {0};
//	double V = 23;
//	static int subwc = V;
//	static const double D = 15, M = 0.668, H = 1.55;
//	if(!prev_power_initialized)
//	{
//		prev_power_initialized = true;
//		for(int i = 0; i < SPECTRUM_SIZE; ++i)
//		{
//			P[i] = in[0][i] * in[0][i] + in[1][i] * in[1][i];
//		}
//	}

//	// boucle principale sur chaque frequence
//	for(int k = 0; k < SPECTRUM_SIZE; ++k)
//	{
//		double current_power = in[0][k] * in[0][k] + in[1][k] * in[1][k];
//		double prevpsum = 0, ysum = 0, Qsum = 0;
//		for(int i = 0; i < SPECTRUM_SIZE; ++i)
//		{
//			prevpsum += P[i];
//			ysum += in[0][i] * in[0][i] + in[1][i] * in[1][i];
//			Qsum += 1. / Q[i];
//		}
//		Qsum -= 1. / Q[k]; // car on le rajoute après
//		double atildec = 1. / (1. + pow(prevpsum / ysum - 1, 2));
//		ac[k] = 0.7 * ac[k] + 0.3 * std::max(0.7, atildec);
//		double a = 0.96 * ac[k] / (1. + pow(P[k] / sigma2[k] - 1, 2));

//		P[k] = a * P[k] + (1. - a) * current_power;

//		double B = a * a;
//			   B = std::max(B, 0.8);
//		nonP[k]  = B * nonP[k]  + (1. - B) * P[k];
//		nonP2[k] = B * nonP2[k] + (1. - B) * P[k] * P[k];

//		double var_P = nonP2[k] - pow(nonP[k], 2);
//		double Qeq = var_P / (2 * pow(sigma2[k], 2));
//		Q[k] = std::max(2., Qeq);
//		Qsum += 1. / Qeq;
//		double Qtilde = (Q[k] - 2. * M) / (1. - M);
//		double nonQinv = 1 / (FFT_SIZE * Qsum);
//		double bc = 1 + 2.12 * sqrt(nonQinv);
//		double bmin = 1 + (D - 1) * 2 / Qtilde;
//		double bmin_sub = -1;

//		k_mod[k] = 0;
//		if(P[k] * bmin * bc < actmin[0][k])
//		{
//			actmin[0][k] = P[k] * bmin * bc;
//			actmin_sub[0][k] = P[k] * bmin_sub * bc;
//			k_mod[k] = 1;
//		}

//		if(subwc == V)
//		{
//			if(k_mod[k] == 1)
//			{
//				lmin_flag[k] = 0;
//			}
//			// sauver actmin(lambda, k)
//			// trouver Pmin_u = min sur les U dernières valeurs de actmin stockées
//			double noise_slope_max = 1.2;
//			if(nonQinv < 0.03) noise_slope_max = 8;
//			else if(nonQinv < 0.05) noise_slope_max = 4;
//			else if(nonQinv < 0.06) noise_slope_max = 2;

//			if (lmin_flag[k]
//			 && actmin_sub[0][k] < noise_slope_max * Pmin_u[k]
//			 && actmin_sub[0][k] > Pmin_u[k])
//			{
//				// remplacer toutes les valeurs de actmin par actmin_sub
//				lmin_flag[k] = 0;
//				subwc = 1;
////				actmin[k] = max(actmin...);
////				actmin_sub[k] = max(actmin_sub...);
//			}
//		}
//		else
//		{
//			if(subwc > 1)
//			{
//				if(k_mod[k] == 1)
//				{
//					lmin_flag[k] = 1;
//				}
//				sigma2[k] = std::min(actmin_sub[0][k], Pmin_u[k]);
//				Pmin_u[k] = sigma2[k];
//			}
//		}
//		++subwc;
//	}
//}


NoiseEstimator::NoiseEstimator(SpectralSubtractor *parent):
	parent(parent)
{ }
#include <QDebug>
void NoiseEstimator::estimationHandler(SubtractionConfiguration& config, bool reinit)
{
	// noise estimation
	switch (config.noiseEstimationAlgorithm)
	{
		case SubtractionConfiguration::NoiseEstimationAlgorithm::Simple:
			simpleEstimation(config.spectrum, config.noise_power, reinit);
			break;

		case SubtractionConfiguration::NoiseEstimationAlgorithm::Martin:
			if(config.spectralSubtractionAlgorithm == SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach)
				fftw_execute(config.plan_fw_windowed); // A utiliser dans martinEstimation si GA ?

			if(config.useOLA)
				martinEstimation(config.windowed_spectrum, parent->spectrumSize, config.noise_power, config.ola_frame_increment, reinit);
			else
				martinEstimation(config.windowed_spectrum, parent->spectrumSize, config.noise_power, config.frame_increment, reinit);
			break;

		case SubtractionConfiguration::NoiseEstimationAlgorithm::Wavelets:
			waveletEstimation(config, reinit);
			break;

		default:
			throw "critical";
	}
}

bool NoiseEstimator::waveletEstimation(SubtractionConfiguration& config, bool reinit)
{
	static bool computeMax = false;
	if(reinit) computeMax = false;
	bool reestimated = simpleEstimation(config.spectrum, config.noise_power, reinit);

	// 1° copy noise_power into noise_power_reest
	std::copy_n(config.noise_power, parent->spectrumSize, config.noise_power_reest);

	if(!reestimated)
	{
		std::copy_n(config.spectrum, parent->spectrumSize, config.tmp_spectrum);
		// 2° SS with noise_power_reest
		switch (config.spectralSubtractionAlgorithm)
		{
			case SubtractionConfiguration::SpectralSubtractionAlgorithm::Standard:
				parent->subtraction(config, config.tmp_spectrum, config.noise_power_reest);
				break;
			case SubtractionConfiguration::SpectralSubtractionAlgorithm::EqualLoudness:
				parent->subtraction_el(config, config.tmp_spectrum, config.noise_power_reest);
				break;
			case SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach:
				parent->geom_algo(config.tmp_spectrum, config.noise_power_reest, reinit);
				break;
			default:
				throw "critical";
		}

		fftw_execute(config.plan_bw_temp);
		// 3° Compute CWT and reestimate noise
		cwt_noise_estimator.estimate(config.tmp_out, config.noise_power_reest, computeMax);
		computeMax = false;
	}
	else
	{
		// Compute the max. of the frame
		computeMax = true;
	}

	return reestimated;
}

void NoiseEstimator::writeSimpleCWT(double *signal)
{
	cwt_noise_estimator.writeSimpleCWT(signal);
}
