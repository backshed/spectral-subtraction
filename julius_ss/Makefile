LIBSENT=../julius-4.2.3/libsent
LIBJULIUS=../julius-4.2.3/libjulius
LIBJLS=../julius-4.2.3/julius

CPPFLAGS=-std=c++11 -fopenmp -O3 -march=native -D_GLIBCXX_PARALLEL
LDFLAGS= -L../output/ -lnoisered -L.  -L$(LIBJULIUS) -L$(LIBJLS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs` -ljls -ljulius -lsent -lcwt -lfftw3 -lasound

all:
	g++-4.8 $(CPPFLAGS) alsa.cpp subWrapper.cpp main.cpp -o ../output/juliusSub $(LDFLAGS)

clean:
	-rm -rf ../output/juliusSub
