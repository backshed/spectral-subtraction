#include "batchprocessing.h"
#include "ui_batchprocessing.h"
#include <QKeyEvent>
#include <QMimeData>
#include <QClipboard>
#include <QProgressDialog>
#include <QDebug>
#define EPSILON 0.0001

BatchProcessing::BatchProcessing(DataHolder *config, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BatchProcessing),
	iterations_vect(2)
{
	ui->setupUi(this);
	connect(ui->Run, SIGNAL(pressed()), this, SLOT(run()));
	connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(table_itemSelectionChanged()));
	this->config = config;

	std::iota(iterations_vect.begin(), iterations_vect.end(), 1);
}

BatchProcessing::~BatchProcessing()
{
	delete ui;
}

void BatchProcessing::run()
{
	ui->Run->setDisabled(true);
	int count = 0;


	ui->tableWidget->clearContents();
	while(ui->tableWidget->rowCount() > 0)
		ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);


	if(config->model == DataHolder::STANDARD)
	{
		int progressMax = (( ui->AlphaEnd->value() -  ui->AlphaBegin->value()) / ui->AlphaStep->value()) *
						  (( ui->BetaEnd->value() -  ui->BetaBegin->value()) / ui->BetaStep->value()) *
						  iterations_vect.size();
		QProgressDialog progress("Processing...", "Abort", 0, progressMax, 0);
		progress.setWindowModality(Qt::WindowModal);

		for(double alpha = ui->AlphaBegin->value();
			alpha < ui->AlphaEnd->value() +  ui->AlphaStep->value() - EPSILON;
			alpha += ui->AlphaStep->value())
		{
			for(double beta = ui->BetaBegin->value();
				beta < ui->BetaEnd->value() + ui->BetaStep->value() - EPSILON;
				beta += ui->BetaStep->value())
			{
				for(uint it = 0; it < iterations_vect.size(); ++it )
				{
					if(progress.wasCanceled())
						goto OutOfThisLoop;
					config->alphaBsc = alpha;
					config->betaBsc = beta;
					config->iterations = iterations_vect[it];

					emit process();
					ui->tableWidget->insertRow(count);
					ui->tableWidget->setItem(count, 0, new QTableWidgetItem(QString::number(alpha)));
					ui->tableWidget->setItem(count, 1, new QTableWidgetItem(QString::number(beta)));
					ui->tableWidget->setItem(count, 2, new QTableWidgetItem("n/a"));
					ui->tableWidget->setItem(count, 3, new QTableWidgetItem("n/a"));
					ui->tableWidget->setItem(count, 4, new QTableWidgetItem(QString::number(iterations_vect[it])));
					ui->tableWidget->setItem(count, 5, new QTableWidgetItem(NRR));
					ui->tableWidget->setItem(count, 6, new QTableWidgetItem(SDR));

					progress.setValue(count);
					++count;
				}
				if( ui->BetaStep->value() == 0.0 ) break;
			}
			if( ui->AlphaStep->value() == 0.0 ) break;
		}
	}
	else if(config->model == DataHolder::EQUAL_LOUDNESS)
	{
		int progressMax = (( ui->AlphaEnd->value() -  ui->AlphaBegin->value()) / ui->AlphaStep->value()) *
						  (( ui->BetaEnd->value() -  ui->BetaBegin->value()) / ui->BetaStep->value()) *
						  (( ui->AlphaWeightEnd->value() -  ui->AlphaWeightBegin->value()) / ui->AlphaWeightStep->value()) *
						  (( ui->BetaWeightEnd->value() -  ui->BetaWeightBegin->value()) / ui->BetaWeightStep->value()) *
						  iterations_vect.size();
		QProgressDialog progress("Processing...", "Abort", 0, progressMax, 0);
		progress.setWindowModality(Qt::WindowModal);

		for(double alpha = ui->AlphaBegin->value();
			alpha < ui->AlphaEnd->value() +  ui->AlphaStep->value() - EPSILON;
			alpha += ui->AlphaStep->value())
		{
			for(double beta = ui->BetaBegin->value();
				beta < ui->BetaEnd->value() + ui->BetaStep->value() - EPSILON;
				beta += ui->BetaStep->value())
			{
				for(double alphawt = ui->AlphaWeightBegin->value();
					alphawt < ui->AlphaWeightEnd->value() + ui->AlphaWeightStep->value() - EPSILON;
					alphawt += ui->AlphaWeightStep->value())
				{
					for(double betawt = ui->BetaWeightBegin->value();
						betawt < ui->BetaWeightEnd->value() + ui->BetaWeightStep->value() - EPSILON;
						betawt += ui->BetaWeightStep->value())
					{

						for(uint it = 0; it < iterations_vect.size(); ++it )
						{
							if(progress.wasCanceled())
								goto OutOfThisLoop;
							config->alphaBsc = alpha;
							config->betaBsc = beta;
							config->iterations = iterations_vect[it];

							emit process();
							ui->tableWidget->insertRow(count);
							ui->tableWidget->setItem(count, 0, new QTableWidgetItem(QString::number(alpha)));
							ui->tableWidget->setItem(count, 1, new QTableWidgetItem(QString::number(beta)));
							ui->tableWidget->setItem(count, 2, new QTableWidgetItem(QString::number(alphawt)));
							ui->tableWidget->setItem(count, 3, new QTableWidgetItem(QString::number(betawt)));
							ui->tableWidget->setItem(count, 4, new QTableWidgetItem(QString::number(iterations_vect[it])));
							ui->tableWidget->setItem(count, 5, new QTableWidgetItem(NRR));
							ui->tableWidget->setItem(count, 6, new QTableWidgetItem(SDR));

							progress.setValue(count);
							++count;
						}
					}
				}
				if( ui->BetaStep->value() == 0.0 ) break;
			}
			if( ui->AlphaStep->value() == 0.0 ) break;
		}
	}
	else if(config->model == DataHolder::GA)
	{
		int progressMax = iterations_vect.size();
		QProgressDialog progress("Processing...", "Abort", 0, progressMax, 0);
		progress.setWindowModality(Qt::WindowModal);


		for(uint it = 0; it < iterations_vect.size(); ++it )
		{
			if(progress.wasCanceled())
				goto OutOfThisLoop;

			config->iterations = iterations_vect[it];

			emit process();
			ui->tableWidget->insertRow(count);
			ui->tableWidget->setItem(count, 0, new QTableWidgetItem("n/a"));
			ui->tableWidget->setItem(count, 1, new QTableWidgetItem("n/a"));
			ui->tableWidget->setItem(count, 2, new QTableWidgetItem("n/a"));
			ui->tableWidget->setItem(count, 3, new QTableWidgetItem("n/a"));
			ui->tableWidget->setItem(count, 4, new QTableWidgetItem(QString::number(iterations_vect[it])));
			ui->tableWidget->setItem(count, 5, new QTableWidgetItem(NRR));
			ui->tableWidget->setItem(count, 6, new QTableWidgetItem(SDR));

			progress.setValue(count);
			++count;
		}


	}
	OutOfThisLoop:
	ui->Run->setEnabled(true);


}

void BatchProcessing::sNRR(QString s)
{
	 NRR = s;
}

void BatchProcessing::sSDR(QString s)
{
	SDR = s;
}

void BatchProcessing::keyPressEvent(QKeyEvent * event)
{
   //if there is a control-C event copy data to the global clipboard
   if(event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
   {
	  QMimeData * mimeData = new QMimeData();
	  mimeData->setData("text/plain",copyArray);
	  QApplication::clipboard()->setMimeData(mimeData);
   }
}

void BatchProcessing::table_itemSelectionChanged()
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