#ifndef NOISEESTIMATORDATAHOLDER_H
#define NOISEESTIMATORDATAHOLDER_H

#include <vector>
#include <functional>
#include <cwtlib>
#include "area.h"


/**
 * @brief Type for functions that are applied on arr.
 *
 * Basically, these functions should only do very simple operations,
 * like void f(uint i, uint j) { arr[i][j] += 1; }.
 * These operations should be applied only on the given coordinates.
 */
typedef std::function<void (uint, uint)> ArrayValueFilter;

class SubtractionConfiguration;
class CWTNoiseEstimator;

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



class NoiseEstimatorDataHolder
{
		friend class NoiseEstimator;
		friend class CWTNoiseEstimator;
	public:
		NoiseEstimatorDataHolder();
		void reinitialize(SubtractionConfiguration& config);

	private:
		/*** For simple noise estimation ***/
		double noise_rms;

		/*** For wavelet noise estimation ***/
		double cwt_amin = 0;
		double cwt_astp = 0.05;
		double cwt_amax = 64;

		cwtlib::Signal *cwt_signal = nullptr; /**< TODO */
		cwtlib::LinearRangeFunctor *cwt_scales = nullptr; /**< TODO */
		cwtlib::WTransform *cwt_wt = nullptr; /**< TODO */
		Matrix *cwt_arr = nullptr; /**< TODO */
		std::vector<Area *> cwt_areas; /**< TODO */
		ArrayValueFilter *cwt_copyFromWT = nullptr; /**< TODO */

		struct areaParams_
		{
			areaParams_();
			int numAreas; /**< Number of areas */
			double mean; /**< Mean value of these areas */
		};
		std::vector<NoiseEstimatorDataHolder::areaParams_> *cwt_areaParams = nullptr; /**< TODO */

		/*** For martin noise estimation -> ENCAPSULATE THIS ZOMG and make noise_martina friend function of the encapsulating object ***/
		int subwc;
		int segment_number;
		double *yft = nullptr;
		double *p = nullptr;
		double *sn2 = nullptr;
		double *pb = nullptr;
		double *pminu = nullptr;
		double *pb2 = nullptr;
		double *actmin = nullptr;
		double *actminsub = nullptr;

		double *ah = nullptr;
		double *b = nullptr;
		double *qeqi = nullptr;
		double *bmind = nullptr;
		double *bminv = nullptr;
		double *lmin = nullptr;
		double *qisq = nullptr;
		bool *kmod = nullptr;
		bool *lminflag = nullptr;
		double **actbuf = nullptr;

		MartinNoiseParams qq;

		unsigned int nu;
		int ibuf;
		double ac;
		double aca;
		double acmax;
		double amax;
		double aminh;
		double bmax;
		double snrexp;
		double nv, nd;
		double md, hd, mv, hv;
		double qeqimax;
		double qeqimin;
		double nsms[4];

};

#endif // NOISEESTIMATORDATAHOLDER_H
