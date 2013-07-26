LIBSENT=../julius-4.2.3/libsent
LIBJULIUS=../julius-4.2.3/libjulius

all:
	g++-4.8 -std=c++11 subWrapper.cpp main.cpp -o juliusSub  -L../output/ -lnoisered -L. -ljls -ljulius -lsent `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs` -lcwt -lfftw3
