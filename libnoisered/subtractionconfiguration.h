#ifndef SUBTRACTIONCONFIGURATION_H
#define SUBTRACTIONCONFIGURATION_H

#include <fftw3.h>
#include "defines.h"
#include <mutex>

class SpectralSubtractor;
class NoiseEstimator;
class CWTNoiseEstimator;
/**
 * @brief Holds the data for the spectral subtraction algorithms, as well as the raw audio data.
 *
 */
class SubtractionConfiguration
{
		friend class SpectralSubtractor;
		friend class CWTNoiseEstimator;
		friend class NoiseEstimator;
	public:
		/**
		 * @brief Algorithm used to estimate noise.
		 *
		 */
		enum class NoiseEstimationAlgorithm { Simple, Wavelets, Martin };

		/**
		 * @brief Algorithm used to perform subtraction.
		 *
		 */
		enum class SpectralSubtractionAlgorithm { Standard, EqualLoudness, GeometricApproach};

		/**
		 * @brief Constructor
		 *
		 * @param fft_Size Wanted size of FFT. 256 or 512 are good choices. Must be a power of two for good performance.
		 * @param sampling_Rate Sampling rate of the audio.
		 */
		SubtractionConfiguration(int fft_Size, int sampling_Rate);

		/**
		 * @brief Destructor.
		 *
		 */
		~SubtractionConfiguration();

		/**
		 * @brief Returns the modified buffer (the subtracted one).
		 *
		 * @return double Pointer to the modified buffer.
		 */
		double *getData();

		/**
		 * @brief Returns the size of the internal buffer / audio file.
		 *
		 * @return unsigned int size.
		 */
		unsigned int getSize();

		/**
		 * @brief Returns the original buffer, to perform NRR computation for instance.
		 *
		 * @return double Pointer to the original buffer.
		 */
		double *getNoisyData();

		/**
		 * @brief Reads a file into the internal buffer.
		 *
		 * @param str Path to the file.
		 * @return unsigned int Size of the file.
		 */
		unsigned int readFile(char *str);

		/**
		 * @brief Reads a buffer into the internal buffer.
		 *
		 * @param buffer Short buffer to read from.
		 * @param length Length of the buffer.
		 * @return unsigned int Length of the buffer (useless?).
		 */
		unsigned int readBuffer(short *buffer, int length);

		/**
		 * @brief Writes into a buffer.
		 *
		 * @param buffer Pointer to the buffer to write to.
		 */
		void writeBuffer(short *buffer);

		/**
		 * @brief Undoes all change on the audio data.
		 *
		 */
		void reinitData();

		/**
		 * @brief Returns alpha.
		 *
		 * @return double alpha.
		 */
		double getAlpha() const;

		/**
		 * @brief Sets alpha.
		 *
		 * @param value alpha.
		 */
		void setAlpha(double value);

		/**
		 * @brief Returns beta.
		 *
		 * @return double beta.
		 */
		double getBeta() const;

		/**
		 * @brief Sets beta.
		 *
		 * @param value beta.
		 */
		void setBeta(double value);

		/**
		 * @brief Returns weighted alpha, for Equal-Loudness algorithm.
		 *
		 * @return double alpha_wt.
		 */
		double getAlphawt() const;

		/**
		 * @brief Sets weighted alpha, for Equal-Loudness algorithm.
		 *
		 * @param value alpha_wt.
		 */
		void setAlphawt(double value);

		/**
		 * @brief Returns weighted beta, for Equal-Loudness algorithm.
		 *
		 * @return double beta_wt.
		 */
		double getBetawt() const;

		/**
		 * @brief Sets weighted beta, for Equal-Loudness algorithm.
		 *
		 * @param value beta_wt.
		 */
		void setBetawt(double value);

		/**
		 * @brief Returns the number of iterations of the algorithm.
		 *
		 * @return int Number of iterations.
		 */
		int getIterations() const;

		/**
		 * @brief Sets the number of iterations of the algorithm.
		 *
		 * @param value Number of iterations.
		 */
		void setIterations(int value);

		/**
		 * @brief Returns the chosen noise estimation algorithm.
		 *
		 * @return NoiseEstimationAlgorithm Chosen noise estimation algorithm.
		 */
		NoiseEstimationAlgorithm getNoiseEstimationAlgorithm() const;

		/**
		 * @brief Sets the chosen noise estimation algorithm.
		 *
		 * @param value Noise estimation algorithm.
		 */
		void setNoiseEstimationAlgorithm(const NoiseEstimationAlgorithm &value);

		/**
		 * @brief Returns the chosen spectral subtraction algorithm.
		 *
		 * @return SpectralSubtractionAlgorithm Chosen spectral subtraction algorithm.
		 */
		SpectralSubtractionAlgorithm getSpectralSubtractionAlgorithm() const;

		/**
		 * @brief Sets the chosen spectral subtraction algorithm.
		 *
		 * @param value Spectral subtraction algorithm.
		 */
		void setSpectralSubtractionAlgorithm(const SpectralSubtractionAlgorithm &value);

		/**
		 * @brief Returns the chosen FFT size.
		 *
		 * @return unsigned int FFT size.
		 */
		unsigned int getFftSize() const;

		/**
		 * @brief Sets the FFT size.
		 *
		 * Resets most arrays and data, because their size and meaning changes.
		 * Keeps the original and processed array, however, because they are not dependent on this parameter.
		 *
		 * @param value FFT size.
		 */
		void setFftSize(unsigned int value);

		/**
		 * @brief Returns the size of the spectrum.
		 *
		 * We use FFTW r2c functions so the size is FFT_SIZE / 2 + 1.
		 *
		 * @return unsigned int Spectrum size.
		 */
		unsigned int getSpectrumSize() const;

		/**
		 * @brief Returns the sampling rate.
		 *
		 * @return unsigned int Sampling rate.
		 */
		unsigned int getSamplingRate() const;

		/**
		 * @brief Sets the sampling rate.
		 *
		 * Resets most arrays and data, because their size and meaning changes.
		 * Keeps the original and processed array, however, because they are not dependent on this parameter.
		 *
		 * @param value Sampling rate.
		 */
		void setSamplingRate(unsigned int value);

		/**
		 * @brief Reads parameters from subtraction.conf file.
		 *
		 * Syntax of this file : one parameter per line, on this order :
			alpha
			beta
			alphawt
			betawt
			iterations
			noise algo (std / martin / wavelets)
			algo (std / el / ga)
		 *
		 */
		void readParametersFromFile();

	private:
		/**
		 * @brief Deletes most of the arrays.
		 *
		 * Used when essential parameters like FS or fft size change.
		 *
		 */
		void clean();

		/**
		 * @brief Initializes most of the data.
		 *
		 */
		void initStructs();

		/**
		 * @brief Puts a signed 16bit integer (red book) between the -1 / 1 range in double.
		 *
		 * @param x Integer to convert.
		 * @return double Corresponding floating point value.
		 */

		static inline double ShortToDouble(short x);
		/**
		 * @brief Puts a double between -1 and 1 into a 16 bit signed integer (red book).
		 *
		 * @param x Double to convert.
		 * @return short Corresponding short value.
		 */
		static inline short DoubleToShort(double x);

		//*** Data copying algorithms ***//
		/**
		 * @brief Copies untouched value into inner fft-sized buffer for transformation.
		 *
		 * @param pos Sample where the copy must start.
		 */
		void copyInputSimple(int pos);

		/**
		 * @brief Copies subtracted values into file or large buffer after transformation.
		 *
		 * @param pos Sample where the copy must start.
		 */
		void copyOutputSimple(int pos);

		/**
		 * @brief Copies untouched value into inner fft-sized buffer for transformation.
		 *
		 * Uses the overlap-add method (adds a zero-padding).
		 *
		 * @param pos
		 */
		void copyInputOLA(int pos);

		/**
		 * @brief Copies subtracted values into file or large buffer after transformation.
		 *
		 * Uses the overlap-add method (+= instead of =).
		 *
		 * NOT SUITED FOR THREADING (yet).
		 *
		 * @param pos
		 */
		void copyOutputOLA(int pos);



		//*** Subtraction "main" parameters***//
		NoiseEstimationAlgorithm estimationAlgo; /**< TODO */
		SpectralSubtractionAlgorithm subtractionAlgo; /**< TODO */

		unsigned int fftSize; /**< TODO */
		unsigned int spectrumSize; /**< TODO */
		unsigned int samplingRate; /**< TODO */

		double alpha; /**< TODO */
		double beta; /**< TODO */
		double alphawt; /**< TODO */
		double betawt; /**< TODO */
		unsigned int iterations; /**< TODO */

		double *data; /**< TODO */
		double *origdata; /**< TODO */
		unsigned int tab_length; /**< TODO */

		bool useOLA; // recommended with GA /**< TODO */
		unsigned int ola_frame_increment; /**< TODO */
		unsigned int frame_increment; /**< TODO */

		//*** used inside algos ***///
		// Arrays used for storing data
		double *in; /**< TODO */
		double *windowed_in; /**< TODO */
		double *out; /**< TODO */
		double *tmp_out; /**< TODO */
		fftw_complex *spectrum; /**< TODO */
		fftw_complex *tmp_spectrum; /**< TODO */
		fftw_complex *windowed_spectrum; /**< TODO */
		double *noise_power; /**< TODO */
		double *noise_power_reest; /**< TODO */

		// FFTW plans
		fftw_plan plan_fw; /**< TODO */
		fftw_plan plan_fw_windowed; /**< TODO */
		fftw_plan plan_bw; /**< TODO */
		fftw_plan plan_bw_temp; /**< TODO */

		//*** For threading ***//
		std::mutex ola_mutex; /**< TODO */


};

#endif // SUBTRACTIONCONFIGURATION_H
