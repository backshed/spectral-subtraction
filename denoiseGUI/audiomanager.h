#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QWidget>
#include <QtMultimedia/QAudioOutput>
#include <QFile>
#include <QDir>
#include "dataholder.h"

#include <subtraction_manager.h>
#include <QBuffer>

class AudioManager : public QWidget
{
	Q_OBJECT
public:
	explicit AudioManager(DataHolder* config, QWidget *parent = 0);
	QByteArray *getOutArray();
	QBuffer* getAudioBuffer();

signals:
	void sNRR(QString);
	void sSDR(QString);

public slots:
	void loadSource(QString src);
	void loadOriginal(QString src);
	void playNoise();
	void stopNoise();
	void playRed();
	void stopRed();
	void handleStateChanged(QAudio::State newState);
	void exec();


private:
	QBuffer* audioBuffer;
	DataHolder *data;
	QFile *noiseFile;
	QFile *origFile;
	QAudioFormat format;
	QAudioOutput *audioOut;

	bool loaded;

	SubtractionManager s_mgr;
	double *origData;


};

#endif // AUDIOMANAGER_H
