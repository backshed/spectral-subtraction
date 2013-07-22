#ifndef DATAHOLDER_H
#define DATAHOLDER_H
#include <QObject>
class DataHolder: public QObject

{
		Q_OBJECT

	public slots:
		void setModel(int);
		void setEnableWavelets(int value);

	public:
		enum Model { STANDARD, EQUAL_LOUDNESS, GA };
		DataHolder();
		~DataHolder();

		double alphaBsc, betaBsc, alphaWt, betaWt;
		int iterations;
		bool useMartin, enableWavelets;
		DataHolder::Model model;

		bool getEnableWavelets() const;

};

#endif // DATAHOLDER_H
