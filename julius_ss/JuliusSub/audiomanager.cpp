#include "audiomanager.h"
#include <QDebug>


AudioManager::AudioManager()
{
	QList<QAudioDeviceInfo> ql = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for(QAudioDeviceInfo dev : ql) qDebug() << dev.deviceName();

	audioBuffer = new QBuffer();
	audioBuffer->open(QBuffer::ReadWrite);

	format.setSampleRate(16000);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

        QDataStream stream(audioBuffer);
        stream.setByteOrder(QDataStream::LittleEndian);

	audioBuffer->seek(0);
        for(auto i = 0U; i < 8192; ++i)
        {
                stream << 0;
        }
	audioBuffer->seek(0);

	audioOut = new QAudioOutput(format, this);

	//connect(audioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));	
}

void AudioManager::writeAudio(short* ext_buffer, unsigned int len)
{

	qint64 pos = audioBuffer->pos();
	audioBuffer->seek(audioBuffer->size());

	QDataStream stream(audioBuffer);
	stream.setByteOrder(QDataStream::LittleEndian);

	for(auto i = 0U; i < len; ++i)
	{
		stream << ext_buffer[i];
	}
	audioBuffer->seek(pos);

	if(audioOut->state() != QAudio::ActiveState)
		play();
}


void AudioManager::play()
{
	audioOut->start(audioBuffer);
}

void AudioManager::stop()
{
	audioOut->stop();
	audioBuffer->seek(0);
}

void AudioManager::handleStateChanged(QAudio::State newState)
{
	switch (newState) {
		case QAudio::IdleState:
			audioOut->start(audioBuffer);
			break;

		case QAudio::StoppedState:
			audioOut->start(audioBuffer);
			break;

		default:
			// ... other cases as appropriate
			break;
	}
}
