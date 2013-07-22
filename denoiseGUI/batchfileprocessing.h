#ifndef BATCHFILEPROCESSING_H
#define BATCHFILEPROCESSING_H

#include <QDialog>
#include "dataholder.h"
#include <vector>

namespace Ui {
	class BatchFileProcessing;
}

class BatchFileProcessing : public QDialog
{
		Q_OBJECT

	public:
		explicit BatchFileProcessing(DataHolder* config, QWidget *parent = 0);
		~BatchFileProcessing();

	protected:
		void keyPressEvent(QKeyEvent * event);
	signals:
		void process();
	public slots:
		void run();
		void sNRR(QString);
		void sSDR(QString);
		void table_itemSelectionChanged();

	private:
		Ui::BatchFileProcessing *ui;
		DataHolder* config;
		QString NRR, SDR;

		QByteArray copyArray;
};

#endif // BATCHFILEPROCESSING_H
