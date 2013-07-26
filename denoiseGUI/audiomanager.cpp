#include "audiomanager.h"

#include <eval.h>
#include <cstring>
#include <QDebug>
#include <QMessageBox>
#include <QtAlgorithms>

AudioManager::AudioManager(DataHolder *config, QWidget *parent) :
	QWidget(parent)
{
	s = new SpectralSubtractor();

	s_data = new SubtractionConfiguration(512, 16000);

	loaded = false;
	noiseFile = 0;
	origFile = 0;
	origData = nullptr;
	audioBuffer = new QBuffer();
	data = config;

	s->initialize(*s_data);

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

	s_data->setIterations(data->iterations);

	s_data->setAlpha(data->alphaBsc);
	s_data->setBeta(data->betaBsc);
	s_data->setAlphawt(data->alphaWt);
	s_data->setBetawt(data->betaWt);

	if(data->useMartin)
		s_data->setNoiseEstimationAlgorithm(SubtractionConfiguration::NoiseEstimationAlgorithm::Martin);
	else if(data->enableWavelets)
		s_data->setNoiseEstimationAlgorithm(SubtractionConfiguration::NoiseEstimationAlgorithm::Wavelets);
	else
		s_data->setNoiseEstimationAlgorithm(SubtractionConfiguration::NoiseEstimationAlgorithm::Simple);

	switch(data->model)
	{
		case DataHolder::STANDARD:
			s_data->setSpectralSubtractionAlgorithm(SubtractionConfiguration::SpectralSubtractionAlgorithm::Standard);
			break;
		case DataHolder::EQUAL_LOUDNESS:
			s_data->setSpectralSubtractionAlgorithm(SubtractionConfiguration::SpectralSubtractionAlgorithm::EqualLoudness);
			break;
		case DataHolder::GA:
			s_data->setSpectralSubtractionAlgorithm(SubtractionConfiguration::SpectralSubtractionAlgorithm::GeometricApproach);
			break;
	}

	s->execute(*s_data);

	emit sNRR(QString("%1").arg(NRR(s_data->getNoisyData(), s_data->getData(), s_data->getSize())));
	if(origData != nullptr)
	{
		emit sSDR(QString("%1").arg(SDR(origData, s_data->getData(), s_data->getSize())));
	}

	audioOut->stop();
	audioBuffer->open(QBuffer::WriteOnly);
	audioBuffer->buffer().clear();

	QDataStream stream(audioBuffer);
	stream.setByteOrder(QDataStream::LittleEndian);
	for(auto i = 0U; i < s_data->getSize(); ++i)
	{
		qint16 r = s_data->getData()[i] * 32768.;
		stream << r;
	}

	audioBuffer->close();
}

void AudioManager::loadSource(QString src)
{
	if(noiseFile != 0) delete noiseFile;
	noiseFile = new QFile(src);
	s_data->readFile(noiseFile->fileName().toLatin1().data());
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
