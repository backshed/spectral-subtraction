#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T11:38:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = noisered
TEMPLATE = lib
CONFIG += staticlib c++11
DESTDIR = $$PWD/../output
LIBS += -lcwt -lfftw3
QMAKE_CXXFLAGS_RELEASE += -fopenmp -O3 -march=native -D_GLIBCXX_PARALLEL

release:DESTDIR = ../build/libnoisered/release
release:OBJECTS_DIR = $${DESTDIR}/.obj
release:MOC_DIR = $${DESTDIR}/.moc
release:RCC_DIR = $${DESTDIR}/.rcc
release:UI_DIR = $${DESTDIR}/.ui

Debug:DESTDIR = ../build/libnoisered/debug
Debug:OBJECTS_DIR = $${DESTDIR}/.obj
Debug:MOC_DIR = $${DESTDIR}/.moc
Debug:RCC_DIR = $${DESTDIR}/.rcc
Debug:UI_DIR = $${DESTDIR}/.ui


SOURCES += \
	subtractionconfiguration.cpp \
	spectral_subtractor.cpp \
	point.cpp \
	pair.cpp \
	noise_estimator.cpp \
	matrix.cpp \
	eval.cpp \
	cwt_noise_estimator.cpp \
	area.cpp \
	learning/standardssstate.cpp \
	learning/standardssaction.cpp \
	learning/sarsa.cpp \
	learning/realaction.cpp

HEADERS += \
	subtractionconfiguration.h \
	spectral_subtractor.h \
	point.h \
	pair.h \
	noise_estimator.h \
	matrix.h \
	eval.h \
	defines.h \
	cwt_noise_estimator.h \
	area.h \
	learning/state.hpp \
	learning/standardssstate.hpp \
	learning/standardssaction.hpp \
	learning/sarsa.hpp \
	learning/realstate.hpp \
	learning/realaction.hpp \
	learning/action.hpp
