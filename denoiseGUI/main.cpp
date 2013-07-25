#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QtMsgHandler>

void customMessageHandler(QtMsgType type,  const QMessageLogContext &, const QString &msg)
{
	QString txt;
	switch (type) {
	case QtDebugMsg:
		txt = QString("Debug: %1").arg(msg);
		break;

	case QtWarningMsg:
		txt = QString("Warning: %1").arg(msg);
	break;
	case QtCriticalMsg:
		txt = QString("Critical: %1").arg(msg);
	break;
	case QtFatalMsg:
		txt = QString("Fatal: %1").arg(msg);
		abort();
	}

	QFile outFile("debuglog.txt");
	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&outFile);
	ts << txt << endl;
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//qInstallMessageHandler(customMessageHandler);
	MainWindow w;
	w.show();

	return a.exec();
}
