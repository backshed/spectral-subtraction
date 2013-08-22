Using this library
------------------

1. Using the algorithms in other softwares : the basic code would look like this : 

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
