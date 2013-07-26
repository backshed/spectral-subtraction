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
	batchprocessing.cpp \
	batchfileprocessing.cpp

HEADERS  += mainwindow.h \
	filemanager.h \
	audiomanager.h \
	dataholder.h \
	batchprocessing.h \
	batchfileprocessing.h \



FORMS    += mainwindow.ui \
	batchprocessing.ui \
	batchfileprocessing.ui


INCLUDEPATH += $$PWD/../libnoisered $$PWD/../libnoisered/learning

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../output/release/ -lnoisered
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../output/debug/ -lnoisered
else:unix:!macx: LIBS += -L$$PWD/../output/ -lnoisered

INCLUDEPATH += $$PWD/../libnoisered
DEPENDPATH += $$PWD/../libnoisered

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../output/release/noisered.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../output/debug/noisered.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../output/libnoisered.a
LIBS += -lfftw3  -lcwt
