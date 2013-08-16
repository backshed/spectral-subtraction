spectral-subtraction
====================

Spectral subtraction library.
Requires FFTW3, CWT Library (cwtlib++) available here : 
http://sourceforge.net/projects/cwtlib/.
The project assumes it is installed as a system-wide library so compile it, and put the headers and static lib in your GCC's path.

Qt5 is needed for testing GUI.
Compiles with g++-4.8 and uses some C++11 features so older compilers might
not work.


To build everything :
- Check the makefile for the julius URL
- Download and configure julius:
      make juliusinit 
- Build everything: (Output is in output/ folder)
      make all

