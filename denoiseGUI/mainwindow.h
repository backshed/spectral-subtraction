#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audiomanager.h"
#include "filemanager.h"
#include "dataholder.h"
#include "batchprocessing.h"
#include "batchfileprocessing.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	friend class AudioManager;
	friend class FileManager;
	friend class BatchFileProcessing;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QBuffer* getAudioBuffer();

public slots:
	void apply();


private:
	Ui::MainWindow *ui;
	FileManager* fileManager;
	AudioManager* audioManager;
	DataHolder* config;
	BatchProcessing* batch;
	BatchFileProcessing * batchFile;
};

#endif // MAINWINDOW_H
