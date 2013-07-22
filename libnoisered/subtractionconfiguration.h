#ifndef SUBTRACTIONCONFIGURATION_H
#define SUBTRACTIONCONFIGURATION_H

#include <fftw3.h>
#include "defines.h"

class SpectralSubtractor;
class NoiseEstimator;
class CWTNoiseEstimator;
class SubtractionConfiguration
{
		friend class SpectralSubtractor;
		friend class CWTNoiseEstimator;
		friend class NoiseEstimator;
	public:
		enum class NoiseEstimationAlgorithm { Simple, Wavelets, Martin };
		enum class SpectralSubtractionAlgorithm { Standard, EqualLoudness, GeometricApproach};
		SubtractionConfiguration(int fft_Size, int sampling_Rate);
		~SubtractionConfiguration();

		double* getData();
		unsigned int getSize();

		double* getNoiseData();

		int read_file(char *str);

		void reinitData();
		double getAlpha() const;
		void setAlpha(double value);

		double getBeta() const;
		void setBeta(double value);

		double getAlphawt() const;
		void setAlphawt(double value);

		double getBetawt() const;
		void setBetawt(double value);

		int getIterations() const;
		void setIterations(int value);

		NoiseEstimationAlgorithm getNoiseEstimationAlgorithm() const;
		void setNoiseEstimationAlgorithm(const NoiseEstimationAlgorithm &value);

		SpectralSubtractionAlgorithm getSpectralSubtractionAlgorithm() const;
		void setSpectralSubtractionAlgorithm(const SpectralSubtractionAlgorithm &value);

		unsigned int getFftSize() const;
		void setFftSize(unsigned int value);

		unsigned int getSpectrumSize() const;

		unsigned int getSamplingRate() const;
		void setSamplingRate(unsigned int value);


	private:
		void clean();
		void initStructs();

		//*** Data copying algorithms ***//
		void copyInputSimple(int pos);
		void copyOutputSimple(int pos);
		void copyInputOLA(int pos);
		void copyOutputOLA(int pos);



		//*** Subtraction parameters***//
		NoiseEstimationAlgorithm noiseEstimationAlgorithm;
		SpectralSubtractionAlgorithm spectralSubtractionAlgorithm;

		unsigned int fftSize;
		unsigned int spectrumSize;
		unsigned int samplingRate;

		double alpha;
		double beta;
		double alphawt;
		double betawt;
		int iterations;

		double* data;
		double* origdata;
		unsigned int filesize;

		bool useOLA; // recommended with GA

		//*** used inside algos ***///
		double *in;
		double *windowed_in;
		double *out;
		double *tmp_out;
		fftw_complex *spectrum;
		fftw_complex *tmp_spectrum;
		fftw_complex *windowed_spectrum;
		double *noise_power;
		double *noise_power_reest;

		fftw_plan plan_fw;
		fftw_plan plan_fw_windowed;
		fftw_plan plan_bw;
		fftw_plan plan_bw_temp;

		unsigned int ola_frame_increment;
		unsigned int frame_increment;

};

#endif // SUBTRACTIONCONFIGURATION_H
