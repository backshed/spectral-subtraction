#-------------------------------------------------
#
# Project created by QtCreator 2013-08-06T18:03:16
#
#-------------------------------------------------

QT       += core

QT       -= gui

CONFIG += c++11
QMAKE_CXX = g++
QMAKE_LINK = g++
QMAKE_LINK_SHLIB = g++
QMAKE_CXXFLAGS += -fopenmp -O3 -march=native -D_GLIBCXX_PARALLEL
QMAKE_LFLAGS += -fopenmp

TARGET =juliusSub
DESTDIR =$$PWD/../../output/
CONFIG   +=console
CONFIG   -=app_bundle

TEMPLATE = app



QMAKE_CXXFLAGS+=-std=c++11 -O3 -march=native

SOURCES += main.cpp \
	subWrapper.cpp \
	audiomanager.cpp

HEADERS += \
	subWrapper.h \
	audiomanager.h

LIBS += -L$$PWD/../../output/ -ljls  -L$$PWD/../../output/ -lnoisered -L$$PWD/../../julius-4.2.3/libjulius -ljulius -L$$PWD/../../julius-4.2.3/libsent -lsent

INCLUDEPATH += $$PWD/../../libnoisered
DEPENDPATH += $$PWD/../../libnoisered

unix:!macx: PRE_TARGETDEPS += $$PWD/../../output/libnoisered.a

unix:!macx: LIBS +=

unix:!macx: PRE_TARGETDEPS += $$PWD/../../output/libjls.a
# message($$QMAKE_TARGET.arch)
# contains(QMAKE_TARGET.arch, arm):{
# message("BeagleBoard compilation")
MOBILITY += multimedia
INCLUDEPATH +=/usr/include/QtMultimediaKit
INCLUDEPATH +=/usr/include/QtMobility
LIBS +=  -lQtMultimediaKit
# }
# linux-g++-64 {
# QT += multimedia
#}

LIBS +=  -lcwt -lfftw3 -ldl -lpthread  -lasound -lz -lsndfile -lm -lpulse -lpulse-simple
