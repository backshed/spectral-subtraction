#include "filemanager.h"
#include "mainwindow.h"
#include "audiomanager.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
FileManager::FileManager(QWidget *parent) :
	QWidget(parent)
{
	loaded = false;
}

void FileManager::open()
{
	QString filename = QFileDialog::getOpenFileName(this, "", "", "RAW Audio (*.raw)");
	if(!filename.isNull())
	{
		qobject_cast<MainWindow*>(parent())->audioManager->loadSource(filename);
		loaded = true;

		emit noisyLoaded(true);
	}

}

void FileManager::openOriginal()
{
	QString filename = QFileDialog::getOpenFileName(this, "", "", "RAW Audio (*.raw)");
	if(!filename.isNull())
	{
		qobject_cast<MainWindow*>(parent())->audioManager->loadOriginal(filename);
		loaded = true;

		emit originalLoaded(true);
	}
}

void FileManager::save()
{
	if(!loaded)
	{
		QMessageBox::warning(0, "Warning!", "Please load file first");
		return;
	}
	QString filename = QFileDialog::getSaveFileName(this, "", "", "RAW Audio (*.raw)");

	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	f.write(qobject_cast<MainWindow *>(parent())->getAudioBuffer()->buffer());
	f.close();
}
