LIBSENT=../julius-4.2.3/libsent
LIBJULIUS=../julius-4.2.3/libjulius
LIBJLS=../julius-4.2.3/julius

CPPFLAGS=-fopenmp -O3 -march=native -D_GLIBCXX_PARALLEL
LDFLAGS= -L../output/ -lnoisered -L.  -L$(LIBJULIUS) -L$(LIBJLS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs` -ljls -ljulius -lsent -lcwt -lfftw3

all:
	g++-4.8 -std=c++11 subWrapper.cpp main.cpp -o ../output/juliusSub $(LDFLAGS)
