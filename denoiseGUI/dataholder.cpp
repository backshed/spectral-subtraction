#include "dataholder.h"
#include <QDebug>
DataHolder::DataHolder()
{
	model = STANDARD;
	useMartin = false;
	enableWavelets = false;
}

void DataHolder::setModel(int m)
{
	model = (Model) m;
}

DataHolder::~DataHolder()
{

}


bool DataHolder::getEnableWavelets() const
{
	return enableWavelets;
}

void DataHolder::setEnableWavelets(int value)
{
	qDebug() << "enabled ? " << (bool) value;
	enableWavelets = value;
}
