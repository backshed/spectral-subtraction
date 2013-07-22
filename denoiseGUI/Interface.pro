#-------------------------------------------------
#
# Project created by QtCreator 2013-06-05T16:30:46
#
#-------------------------------------------------

QT       += core gui multimedia widgets
DESTDIR = $$PWD/../output

TARGET = Interface
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
		mainwindow.cpp \
	filemanager.cpp \
	audiomanager.cpp \
	dataholder.cpp \
		../libnoisered/eval.cpp \
	batchprocessing.cpp \
		../libnoisered/filters.cpp \
		../libnoisered/window.cpp \
	batchfileprocessing.cpp \
	../libnoisered/area.cpp \
	../libnoisered/matrix.cpp \
	../libnoisered/pair.cpp \
	../libnoisered/point.cpp \
	../libnoisered/cwt_noise_estimator.cpp \
	../libnoisered/subtractionconfiguration.cpp \
	../libnoisered/noise_estimator.cpp \
	../libnoisered/spectral_subtractor.cpp

HEADERS  += mainwindow.h \
	filemanager.h \
	audiomanager.h \
	dataholder.h \
		../libnoisered/eval.h \
	batchprocessing.h \
		../libnoisered/filters.h \
		../libnoisered/defines.h \
		../libnoisered/window.h \
	batchfileprocessing.h \
	../libnoisered/area.h \
	../libnoisered/matrix.h \
	../libnoisered/pair.h \
	../libnoisered/point.h \
	../libnoisered/cwt_noise_estimator.h \
	../libnoisered/subtractionconfiguration.h \
	../libnoisered/noise_estimator.h \
	../libnoisered/spectral_subtractor.h


FORMS    += mainwindow.ui \
	batchprocessing.ui \
	batchfileprocessing.ui

unix:LIBS += -lfftw3
win32:LIBS += -lfftw3-3


INCLUDEPATH += $$PWD/../libnoisered

#!!! CWTLIB
unix:!macx|win32: LIBS += -lcwt

INCLUDEPATH += cwtlib/
INCLUDEPATH += $$PWD/../cwtlib/cwtlib
#DEPENDPATH += $$PWD/../cwtlib/cwtlib

unix:!macx: PRE_TARGETDEPS += $$PWD/../cwtlib/libs/libcwt.a
