#all:
#	g++ -std=c++11 -g -lasound -lfftw3 main.cpp audiomanager.cpp ../noisereduction_common/noise_red.cpp ../noisereduction_common/filters.cpp ../noisereduction_common/window.cpp -o alsa
LIBSENT=../libsent
LIBJULIUS=../libjulius

CC=g++
CFLAGS=-g -O3

####
#### When using system-installed libraries
####
CPPFLAGS=-std=c++11 -lfftw3 `libjulius-config --cflags` `libsent-config --cflags`
LDFLAGS=`libjulius-config --libs` `libsent-config --libs`

####
#### When using within-package libraries
####
#CPPFLAGS=-I$(LIBJULIUS)/include -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags` `$(LIBJULIUS)/libjulius-config --cflags`
#LDFLAGS= -L$(LIBJULIUS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs`

############################################################

all: julius-simple

julius-simple: speach_recog.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o julius-simple speach_recog.cpp ../noisereduction_common/noise_red.cpp ../noisereduction_common/noise_estimation.cpp ../noisereduction_common/window.cpp ../noisereduction_common/filters.cpp $(LDFLAGS)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) julius-simple
