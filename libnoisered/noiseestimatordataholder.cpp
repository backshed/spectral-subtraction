#include "noiseestimatordataholder.h"
#include "subtractionconfiguration.h"
#include "noise_estimator.h"
#include <climits>
#include <cmath>

#define AMIN 0                  /* A min  */
#define ASTP 0.05               /* A step */
#define AMAX 64                 /* A max  */

void mh_values(double d, double *m, double *h)
{
	static double dmh[3][18] =
	{
		{1, 2, 5, 8, 10, 15, 20, 30, 40, 60, 80, 120, 140, 160, 180, 220, 260, 300},
		{0, .26, .48, .58, .61, .668, .705, .762, .8, .841, .865, .89, .9, .91, .92, .93, .935, .94},
		{0, 0.15, 0.48, 0.78, 0.98, 1.55, 2, 2.3, 2.52, 3.1, 3.38, 4.15, 4.35, 4.25, 3.9, 4.1, 4.7, 5}
	};
	int ichosen = -1, jchosen = -1;
	for (auto i = 0U; i < 18; ++i)
	{
		if (dmh[0][i] >= d)
		{
			ichosen = i;
			jchosen = i - 1;
			break;
		}
	}
	if (ichosen == -1)
	{
		ichosen = 17;
		jchosen = 17;
	}

	if (d == dmh[0][ichosen])
	{
		*m = dmh[1][ichosen];
		*h = dmh[2][ichosen];
	}
	else
	{
		double qj = std::sqrt(dmh[0][jchosen]);
		double qi = std::sqrt(dmh[0][ichosen]);
		double q  = std::sqrt(d);
		*h = dmh[2][ichosen] + (q - qi) * (dmh[2][jchosen] - dmh[2][ichosen]) / (qj - qi);
		*m = dmh[1][ichosen] + (qi * qj / q - qj) * (dmh[1][jchosen] - dmh[1][ichosen]) / (qi - qj);
	}
}

NoiseEstimatorDataHolder::NoiseEstimatorDataHolder()
{
}

void NoiseEstimatorDataHolder::reinitialize(SubtractionConfiguration &config)
{
	switch (config.estimationAlgo)
	{
		case SubtractionConfiguration::NoiseEstimationAlgorithm::Simple:
			// Simple reestimation
			noise_rms = 100000;
			break;

		case SubtractionConfiguration::NoiseEstimationAlgorithm::Martin:
			// Stuff pour martin reestim.
			auto local_frame_increment = config.useOLA? config.ola_frame_increment : config.frame_increment;

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

			nu      = qq.nu;
			ac      = 1;
			aca     = exp(-local_frame_increment / qq.taca); // smoothing constant for alpha_c in equ (11) = 0.7
			acmax   = aca;          // min value of alpha_c = 0.7 in equ (11) also = 0.7
			amax    = exp(-local_frame_increment / qq.tamax); // max smoothing constant in (3) = 0.96
			aminh   = exp(-local_frame_increment / qq.taminh); // min smoothing constant (upper limit) in (3) = 0.3
			bmax    = exp(-local_frame_increment / qq.tbmax); // max smoothing constant in (20) = 0.8
			snrexp  = -local_frame_increment / qq.tpfall;
			nv      = round(qq.td / (local_frame_increment * qq.nu));    // length of each subwindow in frames
			if (nv < 4)
			{
				// algorithm doesn't work for miniscule frames
				nv = 4;
				nu = std::max(round(qq.td / (local_frame_increment * nv)), 1.);
			}
			subwc = nv;


			nd = nu * nv;           // length of total window in frames

			mh_values(nd, &md, &hd);
			mh_values(nv, &mv, &hv);

			for (int i = 0; i < 4; ++i)
				nsms[i] = pow(10., qq.nsmdb[i] * nv * local_frame_increment / 10.);  // [8 4 2 1.2] in paper

			qeqimax = 1. / qq.qeqmin;  // maximum value of Qeq inverse (23)
			qeqimin = 1. / qq.qeqmax; // minumum value of Qeq per frame inverse

			if (ah) delete ah;
			ah = new double[config.spectrumSize];
			if (b) delete b;
			b = new double[config.spectrumSize];
			if (qeqi) delete qeqi;
			qeqi = new double[config.spectrumSize];
			if (bmind) delete bmind;
			bmind = new double[config.spectrumSize];
			if (bminv) delete bminv;
			bminv = new double[config.spectrumSize];
			if (lmin) delete lmin;
			lmin = new double[config.spectrumSize];
			if (qisq) delete qisq;
			qisq = new double[config.spectrumSize];
			if (kmod) delete kmod;
			kmod = new bool[config.spectrumSize];

			if (yft) delete yft;
			yft = new double[config.spectrumSize];
			if (p) delete p;
			p = new double[config.spectrumSize];
			if (sn2) delete sn2;
			sn2 = new double[config.spectrumSize];
			if (pb) delete pb;
			pb = new double[config.spectrumSize];
			if (pminu) delete pminu;
			pminu = new double[config.spectrumSize];
			if (pb2) delete pb2;
			pb2 = new double[config.spectrumSize];
			if (lminflag) delete lminflag;
			lminflag = new bool[config.spectrumSize];
			if (actmin) delete actmin;
			actmin = new double[config.spectrumSize];
			if (actminsub) delete actminsub;
			actminsub = new double[config.spectrumSize];

			if (actbuf)
			{
				for (auto i = 0U; i < nu; ++i)
				{
					delete actbuf[i];
				}
				delete actbuf;
			}
			actbuf = new double*[nu];
			for (auto i = 0U; i < nu; ++i)
			{
				actbuf[i] = new double[config.spectrumSize];
				for (auto j = 0U; j < config.spectrumSize; ++j)
				{
					actbuf[i][j] = INT_MAX;
				}
			}


		break;

	}







}

NoiseEstimatorDataHolder::areaParams_::areaParams_(): numAreas(0), mean(0) { }
