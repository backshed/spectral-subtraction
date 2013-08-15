TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
QMAKE_CXXFLAGS_RELEASE += -std=c++11 -fopenmp -O3 -march=native -D_GLIBCXX_PARALLEL
QMAKE_LFLAGS_RELEASE += -fopenmp


unix:!macx: LIBS += -L$$PWD/../output/ -lnoisered

INCLUDEPATH += $$PWD/../libnoisered
DEPENDPATH += $$PWD/../libnoisered

unix:!macx: PRE_TARGETDEPS += $$PWD/../output/libnoisered.a
LIBS += -lfftw3  -lcwt
