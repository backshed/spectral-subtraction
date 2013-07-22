#include "batchfileprocessing.h"
#include "ui_batchfileprocessing.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QKeyEvent>
#include <QMimeData>
#include <QClipboard>
#include <QProgressDialog>
#include <QDebug>
BatchFileProcessing::BatchFileProcessing(DataHolder* config, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BatchFileProcessing)

{
	ui->setupUi(this);
	connect(ui->Run, SIGNAL(pressed()), this, SLOT(run()));
	connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(table_itemSelectionChanged()));
	this->config = config;
}

BatchFileProcessing::~BatchFileProcessing()
{
	delete ui;
}

// Parcourir tous les fichiers et leur appliquer à chaque fois les trois algos
void BatchFileProcessing::run()
{
	ui->Run->setDisabled(true);
	int count = 0;


	ui->tableWidget->clearContents();
	while(ui->tableWidget->rowCount() > 0)
		ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);

	QVector<QString> fileVector;
	QDirIterator iterator("../../noise/", QDirIterator::Subdirectories);
	while (iterator.hasNext())
	{
		iterator.next();
		if (!iterator.fileInfo().isDir())
		{
			QString filename = iterator.fileName();
			if (filename.endsWith(".raw"))
			{
				fileVector.push_back(iterator.filePath());
			}
		}
	}

	QVector<QString>::Iterator i;
	QString db;
	QProgressDialog progress("Processing...", "Abort", 0, fileVector.size() * 2, 0);
	auto insert_in_table = [&] (QString algorithm)
	{
		emit process();
		ui->tableWidget->insertRow(count);
		ui->tableWidget->setItem(count, 0, new QTableWidgetItem(*i));
		ui->tableWidget->setItem(count, 1, new QTableWidgetItem(db));
		ui->tableWidget->setItem(count, 2, new QTableWidgetItem(algorithm));
		ui->tableWidget->setItem(count, 3, new QTableWidgetItem(NRR));
		ui->tableWidget->setItem(count, 4, new QTableWidgetItem(SDR));

		progress.setValue(count);
		++count;

	};


	progress.setWindowModality(Qt::WindowModal);

	for(i = fileVector.begin(); i != fileVector.end(); ++i)
	{
		if(progress.wasCanceled())
			goto OutOfThisLoop;

		// Loading the file
		static_cast<MainWindow*>(parent())->audioManager->loadSource(*i);

		// Get the SNR from filename
		int indexus = (*i).lastIndexOf('_') + 1;
		int indexdot = (*i).lastIndexOf('.') - 2 ;
		db = (*i).mid(indexus, indexdot - indexus);

		// Compute
		config->model = DataHolder::STANDARD;
		config->alphaBsc = ui->AlphaStd->value();
		config->betaBsc = ui->BetaStd->value();
		config->iterations = ui->IterStd->value();
		config->setEnableWavelets(false);
		insert_in_table("No WT");

		config->setEnableWavelets(true);
		insert_in_table("WT");
//		config->model = DataHolder::EQUAL_LOUDNESS;
//		config->alphaBsc = ui->AlphaEL->value();
//		config->betaBsc = ui->BetaEL->value();
//		config->alphaWt = ui->AlphaWtEL->value();
//		config->betaWt = ui->BetaWtEL->value();
//		config->iterations = ui->IterEL->value();
//		insert_in_table("Equal-Loudness");

//		config->model = DataHolder::GA;
//		config->iterations = ui->IterGA->value();
//		insert_in_table("Geometric Approach");
	}

OutOfThisLoop:
	ui->Run->setEnabled(true);

}

void BatchFileProcessing::sNRR(QString s)
{
	NRR = s;
}

void BatchFileProcessing::sSDR(QString s)
{
	SDR = s;
}

void BatchFileProcessing::keyPressEvent(QKeyEvent * event)
{
	//if there is a control-C event copy data to the global clipboard
	if(event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
	{
		QMimeData * mimeData = new QMimeData();
		mimeData->setData("text/plain",copyArray);
		QApplication::clipboard()->setMimeData(mimeData);
	}
}

void BatchFileProcessing::table_itemSelectionChanged()
{
	QList<QTableWidgetItem *> selected(ui->tableWidget->selectedItems());
	QTableWidgetItem *item;

	copyArray.clear();

	int prev_line = selected[0]->row();
	foreach(item, selected)
	{
		if(item->row() != prev_line)
		{
			copyArray.append("\n");
			prev_line = item->row();
		}
		copyArray.append(item->text());
		copyArray.append("\t");
	}
}