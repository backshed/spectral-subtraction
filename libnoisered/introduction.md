# Using this library

## Using the algorithms in other software.

The basic code would look like this :

-----

	#include<subtraction_manager.h>

	...

	// Instanciation. 1st param : FFT size, 2nd param : sampling rate. They can be changed afterwards if it is necessary.
	SubtractionManager s_mgr(512, 16000);

	// Smart pointers are used so you should never try to free them yourself, let this to the library.
	// Set an estimation algorithm. Algorithms are in estimation/ folder
	s_mgr.setEstimationImplementation(Estimation_p(new SimpleEstimation(s_mgr)));

	// Set a subtraction algorithm. Algorithms are in subtraction/ folder.
	s_mgr.setSubtractionImplementation(Subtraction_p(new GeometricSpectralSubtraction(s_mgr)));

	// You can also set algorithm-relevant parameters for each algorithm, by instanciating them in their own variable.

	// Read a buffer. Format must be signed short, little-endian (PCM).
	s_mgr.readBuffer(tab, 4096);

	// Or read a RAW file, same format.
	s_mgr.readFile("path/to/file.raw");

	// Compute
	s_mgr.execute();

	// Computed buffer is accessible by:
	double* output = s_mgr.getData();

	// But you can also get write PCM-like buffer by doing :
	s_mgr.writeBuffer(tab);

-----

## Making your own algorithms.

There are three interfaces :
- One for making new estimation algorithms
- One for making new subtraction algorithms
- One for making new FFT implementations

### Estimation algorithm interface. (File: estimation/estimation_algorithm.h)

There are very few methods to reimplement, the most important being :

	bool operator()(std::complex<double>* input_spectrum);

This method is meant to compute the noise estimation. It should be placed
in a buffer inside the class : one is already provided for convenience. (*noise_power*).

----

	double* noisePower();

----

This methode should be reimplemented if you don't use the *noise_power* buffer to
put your estimation. It is used in the main loop to send the estimated power to the subtraction algorithm.

----

	void specific_onFFTSizeUpdate();

----

This method has to be reimplemented, even if it is empty. It is called whenever the FFT size is set or changes,
so generally in this method you change or create new buffers if you use them in your algorithms.

----

	void specific_onDataUpdate();

----

This method has to be reimplemented, even if it is empty. It is called whenever there is obviously a new set data being
given to the algorithm. For instance, if there is a new file. It should reset the noise estimation to its default state.


The *SimpleEstimation* class is a good example.

### Subtraction algorithm interface. (File: subtraction/subtraction_algorithm.h)

There is very few differences with the precedent interface.

----

	void operator()(std::complex<double>* input_spectrum, double* noise_spectrum);

----

This is where the main processing should take place.
The modification is performed in-place.

----

	void onFFTSizeUpdate();
	void onDataUpdate();

----

They serve the same goal as the previous methods, in *Estimation*.

### FFT implementation

The only functions that should be reimplemented are :

----

	virtual void forward() override;
	virtual void backward() override;

----
which performs fw (real to complex) and bw (complex to real) fft.

----
	virtual void updateSize(const unsigned int);
----

Is like onFFTSizeUpdate.

----

	virtual unsigned int spectrumSize();

----

This should be reimplemented if the spectrum size is NOT fft_size / 2 + 1.

The base class fftmanager assumes the FFT will only compute the "useful" array.

Like this : http://www.fftw.org/doc/The-1d-Real_002ddata-DFT.html#The-1d-Real_002ddata-DFT

There are already convenience buffers provided :

- an input buffer, _input.
- an output buffer, _output.
- a spectrum buffer, _spectrum.
