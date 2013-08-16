spectral-subtraction
====================

Spectral subtraction library.
Requires FFTW3, CWT Library (cwtlib++) available here : 
http://sourceforge.net/projects/cwtlib/.

Qt5 is needed for testing GUI.
Compiles with g++-4.8 and uses some C++11 features so older compilers might
not work.


To build everything :
- Check the makefile for the julius URL
- Download and configure julius:
      make juliusinit 
- Build everything: (Output is in output/ folder)
      make all

