#pragma once
#include "estimation_algorithm.h"


/**
 * @brief Performs a noise estimation according to the Martin noise estimation algorithm.
 *
 * This is a port of estnoisem.m file from VOICEBOX.
 * Works best with overlap-add processing, and Geometric Approach algorithm.
 *
 * Refs:
 *    [1] Rainer Martin.
 *        Noise power spectral density estimation based on optimal smoothing and minimum statistics.
 *        IEEE Trans. Speech and Audio Processing, 9(5):504-512, July 2001.
 *    [2] Rainer Martin.
 *        Bias compensation methods for minimum statistics noise power spectral density estimation
 *        Signal Processing, 2006, 86, 1215-1229
 *    [3] Dirk Mauler and Rainer Martin
 *        Noise power spectral density estimation on highly correlated data
 *        Proc IWAENC, 2006
 */
class MartinEstimation: public Estimation
{
	public:
		MartinEstimation(SubtractionManager& configuration);
		virtual ~MartinEstimation();
		virtual Estimation* clone() override;
		virtual bool operator()(std::complex<double>* input_spectrum);

	protected:
		virtual void specific_onFFTSizeUpdate();
		virtual void specific_onDataUpdate();

	private:
		void algo(std::complex<double> *spectrum, int nrf, double *x, double tinc, bool reinit, bool lastcall);
		static void mh_values(double d, double *m, double *h);

		bool _reinit = false;

		/**
		 * @brief The MartinNoiseParams struct
		 * Used for Martin Noise Estimation.
		 */
		struct MartinNoiseParams
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
};
