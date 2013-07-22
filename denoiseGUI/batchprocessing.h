#ifndef BATCHPROCESSING_H
#define BATCHPROCESSING_H

#include <QDialog>
#include <vector>
#include "dataholder.h"
namespace Ui {
	class BatchProcessing;
}

class BatchProcessing : public QDialog
{
		Q_OBJECT

	public:
		explicit BatchProcessing(DataHolder* config, QWidget *parent = 0);
		~BatchProcessing();

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
		Ui::BatchProcessing *ui;
		DataHolder* config;
		QString NRR, SDR;

		QByteArray copyArray;
		std::vector<int> iterations_vect;
};

#endif // BATCHPROCESSING_H
