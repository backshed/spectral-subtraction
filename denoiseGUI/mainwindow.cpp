#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	fileManager(new FileManager(this)),
	config(new DataHolder()),
	batch(new BatchProcessing(config)),
	batchFile(new BatchFileProcessing(config, this))
{
	audioManager = new AudioManager(config, this);
	ui->setupUi(this);

	connect(ui->actionOpen, SIGNAL(triggered()), fileManager, SLOT(open()));
	connect(ui->actionOpen_Original, SIGNAL(triggered()), fileManager, SLOT(openOriginal()));
	connect(ui->actionSave, SIGNAL(triggered()), fileManager, SLOT(save()));
	connect(ui->playOrig, SIGNAL(clicked()), audioManager, SLOT(playNoise()));
	connect(ui->stopOrig, SIGNAL(clicked()), audioManager, SLOT(stopNoise()));

	connect(ui->playReduced, SIGNAL(clicked()), audioManager, SLOT(playRed()));
	connect(ui->stopReduced, SIGNAL(clicked()), audioManager, SLOT(stopRed()));

	connect(ui->apply, SIGNAL(clicked()), this, SLOT(apply()));

	connect(ui->model, SIGNAL(currentIndexChanged(int)), config, SLOT(setModel(int)));

	connect(ui->wavelets, SIGNAL(stateChanged(int)), config, SLOT(setEnableWavelets(int)));

	connect(audioManager, SIGNAL(sNRR(QString)), ui->NRR, SLOT(setText(QString)));
	connect(audioManager, SIGNAL(sSDR(QString)), ui->SDR, SLOT(setText(QString)));
	connect(audioManager, SIGNAL(sNRR(QString)), batch, SLOT(sNRR(QString)));
	connect(audioManager, SIGNAL(sSDR(QString)), batch, SLOT(sSDR(QString)));
	connect(audioManager, SIGNAL(sNRR(QString)), batchFile, SLOT(sNRR(QString)));
	connect(audioManager, SIGNAL(sSDR(QString)), batchFile, SLOT(sSDR(QString)));

	connect(batch, SIGNAL(process()), audioManager, SLOT(exec()));
	connect(batchFile, SIGNAL(process()), audioManager, SLOT(exec()));

	connect(ui->actionBatch_computing, SIGNAL(triggered()), batch, SLOT(exec()));
	connect(ui->actionMultiple_files_processing, SIGNAL(triggered()), batchFile, SLOT(exec()));

	connect(fileManager, SIGNAL(originalLoaded(bool)), ui->checkBox, SLOT(setChecked(bool)));
	connect(fileManager, SIGNAL(noisyLoaded(bool)), ui->checkBox_2, SLOT(setChecked(bool)));
}


MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::apply()
{
	config->alphaBsc = ui->alphaBsc->value();
	config->betaBsc = ui->betaBsc->value();
	config->alphaWt = ui->alphaWt->value();
	config->betaWt = ui->betaWt->value();
	config->iterations = ui->numIterations->value();
	config->useMartin = ui->matinEstimation->isChecked();
	config->model = (DataHolder::Model) ui->model->currentIndex();

	audioManager->exec();
}

QBuffer* MainWindow::getAudioBuffer()
{
	return audioManager->getAudioBuffer();
}
