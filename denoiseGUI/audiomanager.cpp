#include "audiomanager.h"

#include <eval.h>
#include <cstring>
#include <QDebug>
#include <QMessageBox>
#include <QtAlgorithms>

#include <subtraction/algorithms.h>
#include <estimation/algorithms.h>

AudioManager::AudioManager(DataHolder *config, QWidget *parent) :
	QWidget(parent),
	s_mgr(512, 16000)
{

	loaded = false;
	noiseFile = 0;
	origFile = 0;
	origData = nullptr;
	audioBuffer = new QBuffer();
	data = config;


	format.setSampleRate(16000);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	audioOut = new QAudioOutput(format, this);
	connect(audioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}

void AudioManager::exec()
{
	if(!loaded)
	{
		QMessageBox::warning(0, "Warning!", "Please load file first");
		return;
	}

	s_mgr.setIterations(data->iterations);

	if(data->useMartin)
	{
		s_mgr.setEstimationImplementation(new MartinEstimation(s_mgr));
		s_mgr.enableOLA();
	}
	else if(data->enableWavelets)
		s_mgr.setEstimationImplementation(new WaveletEstimation(s_mgr));
	else
		s_mgr.setEstimationImplementation(new SimpleEstimation(s_mgr));


	switch(data->model)
	{
		case DataHolder::STANDARD:
		{
			//TODO memoryleak
			SimpleSpectralSubtraction* subtraction = new SimpleSpectralSubtraction(s_mgr);
			subtraction->setAlpha(data->alphaBsc);
			subtraction->setBeta(data->betaBsc);
			s_mgr.setSubtractionImplementation(subtraction);
			break;
		}
		case DataHolder::EQUAL_LOUDNESS:
		{
			EqualLoudnessSpectralSubtraction* subtraction = new EqualLoudnessSpectralSubtraction(s_mgr);
			subtraction->setAlpha(data->alphaBsc);
			subtraction->setBeta(data->betaBsc);
			subtraction->setAlphawt(data->alphaWt);
			subtraction->setBetawt(data->betaWt);
			s_mgr.setSubtractionImplementation(subtraction);
			break;
		}
		case DataHolder::GA:
		{
			GeometricSpectralSubtraction* subtraction = new GeometricSpectralSubtraction(s_mgr);
			s_mgr.setSubtractionImplementation(subtraction);
			s_mgr.enableOLA();
			break;
		}
	}

	s_mgr.execute();

	emit sNRR(QString("%1").arg(Eval::NRR(s_mgr.getNoisyData(), s_mgr.getData(), s_mgr.getLength())));
	if(origData != nullptr)
	{
		emit sSDR(QString("%1").arg(Eval::SDR(origData, s_mgr.getData(), s_mgr.getLength())));
	}

	audioOut->stop();
	audioBuffer->open(QBuffer::WriteOnly);
	audioBuffer->buffer().clear();

	QDataStream stream(audioBuffer);
	stream.setByteOrder(QDataStream::LittleEndian);
	for(auto i = 0U; i < s_mgr.getLength(); ++i)
	{
		qint16 r = s_mgr.getData()[i] * 32768.;
		stream << r;
	}

	audioBuffer->close();
}

void AudioManager::loadSource(QString src)
{
	if(noiseFile != 0) delete noiseFile;
	noiseFile = new QFile(src);
	s_mgr.readFile(noiseFile->fileName().toLatin1().data());
	loaded = true;
}

void AudioManager::loadOriginal(QString src)
{
	if(origFile != 0) delete origFile;
	origFile = new QFile(src);
	origFile->open(QFile::ReadOnly);
	int len = origFile->size();

	qint16 *origDataBuf = new qint16[len/2];
	origData = new double[len/2];

	origFile->read((char*)origDataBuf, len);
	origFile->close();

	for(int i = 0; i < len / 2; ++i)
		origData[i] = origDataBuf[i] / 32768.;
}

void AudioManager::handleStateChanged(QAudio::State newState)
{
	switch (newState) {
		case QAudio::IdleState:
			// Finished playing (no more data)
			audioOut->stop();
			if(noiseFile->isOpen())
				noiseFile->close();
			break;

		case QAudio::StoppedState:
			// Stopped for other reasons
			if (audioOut->error() != QAudio::NoError) {
				// Error handling
			}
			break;

		default:
			// ... other cases as appropriate
			break;
	}
}


void AudioManager::playRed()
{
	if(!loaded)
	{
		QMessageBox::warning(0, "Warning!", "Please load file first");
		return;
	}
	if(noiseFile->isOpen())
		noiseFile->close();
	audioBuffer->open(QBuffer::ReadOnly);

	audioOut->start(audioBuffer);

}
void AudioManager::stopRed()
{
	if(!loaded)
	{
		QMessageBox::warning(0, "Warning!", "Please load file first");
		return;
	}

	audioOut->stop();
	audioBuffer->seek(0);
	audioBuffer->close();

}

void AudioManager::playNoise()
{
	if(!loaded)
	{
		QMessageBox::warning(0, "Warning!", "Please load file first");
		return;
	}

	noiseFile->open(QIODevice::ReadOnly);
	audioOut->start(noiseFile);
}

void AudioManager::stopNoise()
{
	if(!loaded)
	{
		QMessageBox::warning(0, "Warning!", "Please load file first");
		return;
	}
	audioOut->stop();
	noiseFile->close();
}

QBuffer* AudioManager::getAudioBuffer()
{
	return audioBuffer;
}
