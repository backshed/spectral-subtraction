#ifndef SUBTRACTIONCONFIGURATION_H
#define SUBTRACTIONCONFIGURATION_H

#include <fftw3.h>
#include <memory>

class SpectralSubtractor;
#include "subtraction/algorithms.h"
#include "estimation/algorithms.h"
/**
 * @brief Holds the data for the spectral subtraction algorithms, as well as the raw audio data.
 *
 */
class SubtractionConfiguration
{
		friend class SpectralSubtractor;
	public:
		enum DataSource { File, Buffer };

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

		void prepare();

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


		std::shared_ptr<Subtraction> getSubtractionImplementation() const;
		void setSubtractionImplementation(std::shared_ptr<Subtraction> value);

		unsigned int getFrameIncrement();

		std::shared_ptr<Estimation> getEstimationImplementation() const;
		void setEstimationImplementation(std::shared_ptr<Estimation> value);

		bool bypass();

		DataSource dataSource() const;
		void setDataSource(const DataSource& dataSource);

		bool OLAenabled() const;
		void enableOLA();
		void disableOLA();
		void setOLA(bool val);

		void forwardFFT();
		void backwardFFT();

		fftw_complex *spectrum();

	private:
		/**
		 * @brief Deletes most of the arrays.
		 *
		 * Used when essential parameters like FS or fft size change.
		 *
		 */
		void clean();

		/**
		 * @brief Initializes the needed arrays when a change of FFT size is performed.
		 *
		 */
		void onFFTSizeUpdate();

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
		void copyInput(unsigned int pos);
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

		unsigned int _fftSize; /**< TODO */
		unsigned int _spectrumSize; /**< TODO */
		unsigned int _samplingRate; /**< TODO */

		// Storage
		double *_data; /**< TODO */
		double *_origData; /**< TODO */
		unsigned int _tabLength; /**< TODO */

		bool _useOLA;
		unsigned int ola_frame_increment; /**< TODO */
		unsigned int frame_increment; /**< TODO */

		unsigned int _iterations; /**< TODO */

		// Arrays used for storing data
		double *in = nullptr; /**< TODO */
		double *out = nullptr; /**< TODO */

		fftw_complex *_spectrum = nullptr; /**< TODO */

		fftw_plan plan_fw; /**< TODO */
		fftw_plan plan_bw; /**< TODO */

		// Algorithms
		std::shared_ptr<Subtraction> subtraction = nullptr;
		std::shared_ptr<Estimation>  estimation  = nullptr;

		bool _bypass;

};

#endif // SUBTRACTIONCONFIGURATION_H
