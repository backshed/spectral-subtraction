#include "martin_estimation.h"
#include <cmath>
#include "../math_util.h"
#include <climits>
#include <cfloat>
#include <numeric>
#include <algorithm>
#include <iostream>

#include "subtraction_manager.h"
using namespace std;
void MartinEstimation::mh_values(double d, double *m, double *h)
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


MartinEstimation::MartinEstimation(SubtractionManager& configuration):
	Estimation(configuration)
{
	algorithm = Algorithm::Martin;

}

MartinEstimation::~MartinEstimation()
{

}



void martinEstimation(fftw_complex *spectrum, int nrf, double *x, double tinc, bool reinit)
{
	static int subwc;
	static int segment_number;
	static double *yft = 0;
	static double *p = 0;
	static double *sn2 = 0;
	static double *pb = 0;
	static double *pminu = 0;
	static double *pb2 = 0;
	static double *actmin = 0;
	static double *actminsub = 0;

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

	static MartinNoiseParams qq;

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
	if (reinit)
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

		nu      = qq.nu;
		ac      = 1;
		aca     = exp(-tinc / qq.taca); // smoothing constant for alpha_c in equ (11) = 0.7
		acmax   = aca;          // min value of alpha_c = 0.7 in equ (11) also = 0.7
		amax    = exp(-tinc / qq.tamax); // max smoothing constant in (3) = 0.96
		aminh   = exp(-tinc / qq.taminh); // min smoothing constant (upper limit) in (3) = 0.3
		bmax    = exp(-tinc / qq.tbmax); // max smoothing constant in (20) = 0.8
		snrexp  = -tinc / qq.tpfall;
		nv      = round(qq.td / (tinc * qq.nu));    // length of each subwindow in frames
		if (nv < 4)
		{
			// algorithm doesn't work for miniscule frames
			nv = 4;
			nu = max(round(qq.td / (tinc * nv)), 1.);
		}
		subwc = nv;


		nd = nu * nv;           // length of total window in frames

		MartinEstimation::mh_values(nd, &md, &hd);
		MartinEstimation::mh_values(nv, &mv, &hv);

		for (int i = 0; i < 4; ++i)
			nsms[i] = pow(10., qq.nsmdb[i] * nv * tinc / 10.);  // [8 4 2 1.2] in paper

		qeqimax = 1. / qq.qeqmin;  // maximum value of Qeq inverse (23)
		qeqimin = 1. / qq.qeqmax; // minumum value of Qeq per frame inverse

		if (ah) delete ah;
		ah = new double[nrf];
		if (b) delete b;
		b = new double[nrf];
		if (qeqi) delete qeqi;
		qeqi = new double[nrf];
		if (bmind) delete bmind;
		bmind = new double[nrf];
		if (bminv) delete bminv;
		bminv = new double[nrf];
		if (lmin) delete lmin;
		lmin = new double[nrf];
		if (qisq) delete qisq;
		qisq = new double[nrf];
		if (kmod) delete kmod;
		kmod = new bool[nrf];

		if (yft) delete yft;
		yft = new double[nrf];
		if (p) delete p;
		p = new double[nrf];
		if (sn2) delete sn2;
		sn2 = new double[nrf];
		if (pb) delete pb;
		pb = new double[nrf];
		if (pminu) delete pminu;
		pminu = new double[nrf];
		if (pb2) delete pb2;
		pb2 = new double[nrf];
		if (lminflag) delete lminflag;
		lminflag = new bool[nrf];
		if (actmin) delete actmin;
		actmin = new double[nrf];
		if (actminsub) delete actminsub;
		actminsub = new double[nrf];

		if (actbuf)
		{
			for (int i = 0; i < nu; ++i)
			{
				delete actbuf[i];
			}
			delete actbuf;
		}
		actbuf = new double*[nu];
		for (int i = 0; i < nu; ++i)
		{
			actbuf[i] = new double[nrf];
			for (int j = 0; j < nrf; ++j)
			{
				actbuf[i][j] = INT_MAX;
			}
		}

		MathUtil::compute_power(spectrum, yft, nrf);
		for (int i = 0; i < nrf; ++i)
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
		MathUtil::compute_power(spectrum, yft, nrf);
		segment_number++;
	}



	// Main processing
	double acb = 1. / (1. + pow(accumulate(p, p + nrf, 0.) / accumulate(yft, yft + nrf, 0.) - 1., 2));  // alpha_c-bar(t)  (9)
	ac = aca * ac + (1 - aca) * max(acb, acmax);      // alpha_c(t)  (10)
	for (int i = 0; i < nrf; ++i)
	{
		ah[i] = amax * ac * 1. / (1. + pow(p[i] / sn2[i] - 1., 2));    // alpha_hat: smoothing factor per frequency (11)
	}
	double snr = accumulate(p, p + nrf, 0.) / accumulate(sn2, sn2 + nrf, 0.);

	double localmin = min(aminh, pow(snr, snrexp));
	for (int i = 0; i < nrf; ++i)
	{
		ah[i] = max(ah[i], localmin);       // lower limit for alpha_hat (12)
		p[i] = ah[i] * p[i] + (1 - ah[i]) * yft[i];            // smoothed noisy speech power (3)

		b[i] = min(pow(ah[i], 2.), bmax);              // smoothing constant for estimating periodogram variance (22 + 2 lines)
		pb[i] = b[i] * pb[i] + (1 - b[i]) * p[i];            // smoothed periodogram (20)
		pb2[i] = b[i] * pb2[i] + (1 - b[i]) * pow(p[i], 2);     // smoothed periodogram squared (21)

		qeqi[i] = max(min((pb2[i] - pow(pb[i], 2.)) / (2 * pow(sn2[i], 2.)), qeqimax), qeqimin / segment_number); // Qeq inverse (23)
	}

	double qiav = accumulate(qeqi, qeqi + nrf, 0) / nrf;             // Average over all frequencies (23+12 lines) (ignore non-duplication of DC and nyquist terms)
	double bc = 1. + qq.av * sqrt(qiav);             // bias correction factor (23+11 lines)
	for (int i = 0; i < nrf; ++i)
	{
		bmind[i] = 1. + 2. * (nd - 1.) * (1. - md) / (1. / qeqi[i] - 2. * md);      // we use the simplified form (17) instead of (15)
		bminv[i] = 1. + 2. * (nv - 1.) * (1. - mv) / (1. / qeqi[i] - 2. * mv);      // same expression but for sub windows

		kmod[i] = bc * p[i] * bmind[i] < actmin[i];        // Frequency mask for new minimum

		if (kmod[i])
		{
			actmin[i] = bc * p[i] * bmind[i];
			actminsub[i] = bc * p[i] * bminv[i];
		}
	}


	if (subwc > 0 && subwc < nv)             // middle of buffer - allow a local minimum
	{
		for (int i = 0; i < nrf; ++i)
		{
			lminflag[i] |= kmod[i];     // potential local minimum frequency bins
			pminu[i] = min(actminsub[i], pminu[i]);
			sn2[i] = pminu[i];
		}
	}
	else if (subwc >= nv)                    // end of buffer - do a buffer switch
	{
		ibuf = ibuf % nu;       // increment actbuf storage pointer
		for (int i = 0; i < nrf; ++i)
		{
			actbuf[ibuf][i] = actmin[i];        // save sub-window minimum
		}
		// attention, boucle inverse à l'ordre normal de la matrice (on raisonne en "colonnes")
		for (int i = 0; i < nrf; ++i)
		{
			double tmp = 1;
			for (int j = 0; j < nu; ++j)
			{
				tmp = min(tmp, actbuf[j][i]);
			}
			pminu[i] = tmp;
		}

		int tmp_index = -1;
		for (int i = 0; i < 4; ++i)
		{
			if (qiav < qq.qith[i])
			{
				tmp_index = i;
				break;
			}
		}

		double nsm = nsms[tmp_index];           // noise slope max
		for (int i = 0; i < nrf; ++i)
		{
			lmin[i] = lminflag[i] && !kmod && actminsub[i] < nsm * pminu[i] && actminsub[i] > pminu[i];

			if (lmin[i])
			{
				pminu[i] = actminsub[i];
				for (int j = 0; j < nu; ++j)
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

	for (int i = 0; i < nrf; ++i)
	{
		x[i] = sn2[i];
		qisq[i] = sqrt(qeqi[i]);

		// xs[t][i] = sn2[i] * sqrt(0.266 * (nd + 100 * qisq[i]) * qisq[i] / (1. + 0.005 * nd + 6. / nd) / (0.5 * 1 / qeqi[i] + nd - 1));
	}
}

bool MartinEstimation::operator()(fftw_complex *input_spectrum)
{
	martinEstimation(input_spectrum,  conf.spectrumSize(), noise_power, ((double) conf.getFrameIncrement()) / ((double) conf.getSamplingRate()), reinit);
	reinit = false;
	return true;


	MathUtil::compute_power(input_spectrum, yft, conf.spectrumSize());
	if(segment_number == 0)
	{
		for (auto i = 0U; i < conf.spectrumSize(); ++i)
		{
			p[i] = yft[i];
			sn2[i] = p[i];
			pb[i] = p[i];
			pminu[i] = p[i];
			pb2[i] = p[i] * p[i];
			lminflag[i] = false;
			actmin[i] = DBL_MAX;
			actminsub[i] = DBL_MAX;
		}
	}


	// Main processing
	double acb = 1. / (1. + pow(std::accumulate(p, p + conf.spectrumSize(), 0.) / std::accumulate(yft, yft + conf.spectrumSize(), 0.) - 1., 2));  // alpha_c-bar(t)  (9)
	ac = aca * ac + (1 - aca) * std::max(acb, acmax);      // alpha_c(t)  (10)
	for(auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		ah[i] = amax * ac * 1. / (1. + pow(p[i] / sn2[i] - 1., 2));    // alpha_hat: smoothing factor per frequency (11)
	}
	double snr = std::accumulate(p, p + conf.spectrumSize(), 0.) / std::accumulate(sn2, sn2 + conf.spectrumSize(), 0.);

	double localmin = std::min(aminh, pow(snr, snrexp));
	for(auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		ah[i] = std::max(ah[i], localmin);       // lower limit for alpha_hat (12)
		p[i] = ah[i] * p[i] + (1 - ah[i]) * yft[i];            // smoothed noisy speech power (3)

		b[i] = std::min(pow(ah[i], 2.), bmax);              // smoothing constant for estimating periodogram variance (22 + 2 lines)
		pb[i] = b[i] * pb[i] + (1 - b[i]) * p[i];            // smoothed periodogram (20)
		pb2[i] = b[i] * pb2[i] + (1 - b[i]) * pow(p[i], 2);     // smoothed periodogram squared (21)

		qeqi[i] = std::max(std::min((pb2[i] - pow(pb[i], 2.)) / (2 * pow(sn2[i], 2.)), qeqimax), qeqimin / segment_number); // Qeq inverse (23)
	}

	double qiav = std::accumulate(qeqi, qeqi + conf.spectrumSize(), 0) / conf.spectrumSize();             // Average over all frequencies (23+12 lines) (ignore non-duplication of DC and nyquist terms)
	double bc = 1. + qq.av * sqrt(qiav);             // bias correction factor (23+11 lines)
	for(auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		bmind[i] = 1. + 2. * (nd - 1.) * (1. - md) / (1. / qeqi[i] - 2. * md);      // we use the simplified form (17) instead of (15)
		bminv[i] = 1. + 2. * (nv - 1.) * (1. - mv) / (1. / qeqi[i] - 2. * mv);      // same expression but for sub windows

		kmod[i] = bc * p[i] * bmind[i] < actmin[i];        // Frequency mask for new minimum

		if (kmod[i])
		{
			actmin[i] = bc * p[i] * bmind[i];
			actminsub[i] = bc * p[i] * bminv[i];
		}
	}


	if (subwc > 0 && subwc < nv)             // middle of buffer - allow a local minimum
	{
		for(auto i = 0U; i < conf.spectrumSize(); ++i)
		{
			lminflag[i] |= kmod[i];     // potential local minimum frequency bins
			pminu[i] = std::min(actminsub[i], pminu[i]);
			sn2[i] = pminu[i];
		}
	}
	else if (subwc >= nv)                    // end of buffer - do a buffer switch
	{
		ibuf = ibuf % nu;       // increment actbuf storage pointer
		for(auto i = 0U; i < conf.spectrumSize(); ++i)
		{
			actbuf[ibuf][i] = actmin[i];        // save sub-window minimum
		}
		// attention, boucle inverse à l'ordre normal de la matrice (on raisonne en "colonnes")
		for(auto i = 0U; i < conf.spectrumSize(); ++i)
		{
			double tmp = 1;
			for (auto j = 0U; j < nu; ++j)
			{
				tmp = std::min(tmp, actbuf[j][i]);
			}
			pminu[i] = tmp;
		}

		int tmp_index = -1;
		for(auto i = 0U; i < 4; ++i)
		{
			if (qiav < qq.qith[i])
			{
				tmp_index = i;
				break;
			}
		}

		double nsm = nsms[tmp_index];           // noise slope max
		for(auto i = 0U; i < conf.spectrumSize(); ++i)
		{
			lmin[i] = lminflag[i]
										   && !kmod
										   && actminsub[i] < nsm * pminu[i]
										   && actminsub[i] > pminu[i];

			if (lmin[i])
			{
				pminu[i] = actminsub[i];
				for (auto j = 0U; j < nu; ++j)
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

	for(auto i = 0U; i < conf.spectrumSize(); ++i)
	{
		noise_power[i] = sn2[i];
		qisq[i] = sqrt(qeqi[i]);

		// xs[t][i] = sn2[i] * sqrt(0.266 * (nd + 100 * qisq[i]) * qisq[i] / (1. + 0.005 * nd + 6. / nd) / (0.5 * 1 / qeqi[i] + nd - 1));
	}

	segment_number++;

	return true;
}



void MartinEstimation::onFFTSizeUpdate()
{
	Estimation::onFFTSizeUpdate();

	delete[] ah;
	ah = new double[conf.spectrumSize()];
	delete[] b;
	b = new double[conf.spectrumSize()];
	delete[] qeqi;
	qeqi = new double[conf.spectrumSize()];
	delete[] bmind;
	bmind = new double[conf.spectrumSize()];
	delete[] bminv;
	bminv = new double[conf.spectrumSize()];
	delete[] lmin;
	lmin = new double[conf.spectrumSize()];
	delete[] qisq;
	qisq = new double[conf.spectrumSize()];
	delete[] kmod;
	kmod = new bool[conf.spectrumSize()];

	delete[] yft;
	yft = new double[conf.spectrumSize()];
	delete[] p;
	p = new double[conf.spectrumSize()];
	delete[] sn2;
	sn2 = new double[conf.spectrumSize()];
	delete[] pb;
	pb = new double[conf.spectrumSize()];
	delete[] pminu;
	pminu = new double[conf.spectrumSize()];
	delete[] pb2;
	pb2 = new double[conf.spectrumSize()];
	delete[] lminflag;
	lminflag = new bool[conf.spectrumSize()];
	delete[] actmin;
	actmin = new double[conf.spectrumSize()];
	delete[] actminsub;
	actminsub = new double[conf.spectrumSize()];
}

// reinit
void MartinEstimation::onDataUpdate()
{
	reinit = true;
	return;
	Estimation::onDataUpdate();
	double tinc = conf.getFrameIncrement(); //TODO : this is wrong. It's a time, not a sample number.
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
	qq.qith[3] = DBL_MAX;
	qq.nsmdb[0] = 47.;
	qq.nsmdb[1] = 31.4;
	qq.nsmdb[2] = 15.7;
	qq.nsmdb[3] = 4.1;

	nu      = qq.nu;
	ac      = 1;
	aca     = exp(-tinc / qq.taca); // smoothing constant for alpha_c in equ (11) = 0.7
	acmax   = aca;          // min value of alpha_c = 0.7 in equ (11) also = 0.7
	amax    = exp(-tinc / qq.tamax); // max smoothing constant in (3) = 0.96
	aminh   = exp(-tinc / qq.taminh); // min smoothing constant (upper limit) in (3) = 0.3
	bmax    = exp(-tinc / qq.tbmax); // max smoothing constant in (20) = 0.8
	snrexp  = -tinc / qq.tpfall;
	nv      = round(qq.td / (tinc * qq.nu));    // length of each subwindow in frames
	if (nv < 4)
	{
		// algorithm doesn't work for miniscule frames
		nv = 4;
		nu = std::max(round(qq.td / (tinc * nv)), 1.);

	}
	subwc = nv;

	nd = nu * nv;           // length of total window in frames
	mh_values(nd, &md, &hd);
	mh_values(nv, &mv, &hv);

	for (auto i = 0U; i < 4; ++i)
		nsms[i] = pow(10., qq.nsmdb[i] * nv * tinc / 10.);  // [8 4 2 1.2] in paper

	qeqimax = 1. / qq.qeqmin;  // maximum value of Qeq inverse (23)
	qeqimin = 1. / qq.qeqmax; // minumum value of Qeq per frame inverse


	if (actbuf)
	{
		for (auto i = 0U; i < nu; ++i)
		{
			delete[] actbuf[i];
		}
		delete[] actbuf;
	}
	actbuf = new double*[nu];
	for (auto i = 0U; i < nu; ++i)
	{
		actbuf[i] = new double[conf.spectrumSize()];
		for (auto j = 0U; j < conf.spectrumSize(); ++j)
		{
			actbuf[i][j] = DBL_MAX;
		}
	}
}
