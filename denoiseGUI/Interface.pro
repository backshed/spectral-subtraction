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
	batchfileprocessing.cpp \
	../libnoisered/area.cpp \
	../libnoisered/matrix.cpp \
	../libnoisered/pair.cpp \
	../libnoisered/point.cpp \
	../libnoisered/cwt_noise_estimator.cpp \
	../libnoisered/subtractionconfiguration.cpp \
	../libnoisered/noise_estimator.cpp \
	../libnoisered/spectral_subtractor.cpp \
	../libnoisered/learning/sarsa.cpp \
    ../libnoisered/learning/standardssstate.cpp \
    ../libnoisered/learning/standardssaction.cpp \
    ../libnoisered/learning/realaction.cpp

HEADERS  += mainwindow.h \
	filemanager.h \
	audiomanager.h \
	dataholder.h \
		../libnoisered/eval.h \
	batchprocessing.h \
		../libnoisered/defines.h \
	batchfileprocessing.h \
	../libnoisered/area.h \
	../libnoisered/matrix.h \
	../libnoisered/pair.h \
	../libnoisered/point.h \
	../libnoisered/cwt_noise_estimator.h \
	../libnoisered/subtractionconfiguration.h \
	../libnoisered/noise_estimator.h \
	../libnoisered/spectral_subtractor.h \
    ../libnoisered/learning/state.hpp \
    ../libnoisered/learning/action.hpp \
    ../libnoisered/learning/realstate.hpp \
    ../libnoisered/learning/standardssstate.hpp \
    ../libnoisered/learning/realaction.hpp \
    ../libnoisered/learning/sarsa.hpp \
    ../libnoisered/learning/standardssaction.hpp


FORMS    += mainwindow.ui \
	batchprocessing.ui \
	batchfileprocessing.ui

LIBS += -lfftw3  -lcwt
INCLUDEPATH += $$PWD/../libnoisered $$PWD/../libnoisered/learning
