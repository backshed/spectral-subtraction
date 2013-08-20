#pragma once

#include <fftw3.h>
#include <memory>

#include "subtraction/algorithms.h"
#include "estimation/algorithms.h"
#include "fft/fftmanager.h"
/**
 * @brief Main class.
 *
 */
class SubtractionManager
{
	public:
		enum DataSource { File, Buffer };

		/**
		 * @brief Constructor
		 *
		 * @param fft_Size Wanted size of FFT. 256 or 512 are good choices. Must be a power of two.
		 * @param sampling_Rate Sampling rate of the audio.
		 */
		SubtractionManager(int fft_Size, int sampling_Rate);

		/**
		 * @brief Destructor.
		 *
		 */
		~SubtractionManager();

		/**
		 * @brief Returns the modified buffer (the subtracted one).
		 *
		 * @return double Pointer to the modified buffer.
		 */
		double *getData();

		/**
		 * @brief Generates all the data needed from the given parameters
		 *
		 * For instance, if using EL algorithm, will load loudness countour data from disk.
		 */
		void onDataUpdate();

		/**
		 * @brief Returns the size of the internal buffer / audio file.
		 *
		 * @return unsigned int size.
		 */
		unsigned int getLength();

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
		 * @brief Undoes all change on the processed audio data.
		 *
		 * Effectively copies orig_data into data.
		 *
		 */
		void initDataArray();


		/**
		 * @brief Returns the number of iterations of the algorithm.
		 *
		 * @return int Number of iterations.
		 */
		unsigned int iterations() const;

		/**
		 * @brief Sets the number of iterations of the algorithm.
		 *
		 * @param value Number of iterations.
		 */
		void setIterations(int value);


		/**
		 * @brief Returns the chosen FFT size.
		 *
		 * @return unsigned int FFT size.
		 */
		unsigned int FFTSize() const;

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
		unsigned int spectrumSize() const;

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


		/**
		 * @brief getSubtractionImplementation
		 * @return Used subtraction algorithm.
		 */
		std::shared_ptr<Subtraction> getSubtractionImplementation() const;
		/**
		 * @brief setSubtractionImplementation
		 * @param value Subtraction to use.
		 */
		void setSubtractionImplementation(std::shared_ptr<Subtraction> value);

		/**
		 * @brief getFrameIncrement
		 *
		 * If OLA is enabled, it will only return the "effective" sample number, not including the samples to zero.
		 *
		 * @return Number of samples in each frame.
		 */
		unsigned int getFrameIncrement();

		/**
		 * @brief getEstimationImplementation
		 * @return Used estimation algorithm.
		 */
		std::shared_ptr<Estimation> getEstimationImplementation() const;

		/**
		 * @brief setEstimationImplementation
		 * @param value Estimation to use.
		 */
		void setEstimationImplementation(std::shared_ptr<Estimation> value);

		/**
		 * @brief bypass
		 * @return true if the processing is bypassed, false if not.
		 */
		bool bypass();


		/**
		 * @brief enableOLA Enables overlap-add.
		 */
		void enableOLA();
		/**
		 * @brief disableOLA Disables overlap-add.
		 */
		void disableOLA();
		/**
		 * @brief setOLA To set overlap-add with a boolean.
		 * @param val True to enable, false to disable.
		 */
		void setOLA(bool val);
		/**
		 * @brief OLAenabled
		 * @return true if overlap-add is enabled.
		 */
		bool OLAenabled() const;

		/**
		 * @brief execute Runs the algorithm.
		 */
		void execute();

	private:


		DataSource dataSource() const;

		/**
		 * @brief forwardFFT
		 *
		 * From time domain to spectral domain.
		 */
		void forwardFFT();

		/**
		 * @brief backwardFFT Performs a backward FFT
		 *
		 * From spectral domain to time domain.
		 */
		void backwardFFT();

		std::complex<double> *spectrum();

		/**
		 * @brief Deletes most of the arrays.
		 *
		 */
		void FFTClean();

		/**
		 * @brief Initializes the needed arrays when a change of FFT size is performed.
		 *
		 */
		void onFFTSizeUpdate();



		//*** Data copying algorithms ***//
		/**
		 * @brief copyInput High level handler for input copying.
		 *
		 * @param pos Sample where the copy must start.
		 */
		void copyInput(unsigned int pos);

		/**
		 * @brief copyOutput High level handler for output copying.
		 *
		 * @param pos Sample where the copy must start.
		 */
		void copyOutput(unsigned int pos);

		/**
		 * @brief Copies untouched value into inner fft-sized buffer for transformation.
		 *
		 * @param pos Sample where the copy must start.
		 */
		void copyInputSimple(unsigned int pos);

		/**
		 * @brief Copies subtracted values into file or large buffer after transformation.
		 *
		 * @param pos Sample where the copy must start.
		 */
		void copyOutputSimple(unsigned int pos);

		/**
		 * @brief Copies untouched value into inner fft-sized buffer for transformation.
		 *
		 * Uses the overlap-add method (adds a zero-padding).
		 *
		 * @param pos
		 */
		void copyInputOLA(unsigned int pos);

		/**
		 * @brief Copies subtracted values into file or large buffer after transformation.
		 *
		 * Uses the overlap-add method (+= instead of =).
		 *
		 * NOT SUITED FOR THREADING (yet).
		 *
		 * @param pos
		 */
		void copyOutputOLA(unsigned int pos);


		//*** Members ***//
		DataSource _dataSource;

		unsigned int _samplingRate; /**< TODO */
		std::shared_ptr<FFTManager> fft;


		// Storage
		double *_data = nullptr; /**< TODO */
		double *_origData = nullptr; /**< TODO */
		unsigned int _tabLength = 0; /**< TODO */

		bool _useOLA = false;
		unsigned int _ola_frame_increment; /**< TODO */
		unsigned int _std_frame_increment; /**< TODO */

		unsigned int _iterations = 1; /**< TODO */

		// Algorithms
		std::shared_ptr<Subtraction> subtraction;
		std::shared_ptr<Estimation>  estimation;


		// For measurements
		bool _bypass = false;
};
