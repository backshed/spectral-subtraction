#include "audiomanager.h"
#include <QDebug>


AudioManager::AudioManager()
{
	audioBuffer = new QBuffer();
	audioBuffer->open(QBuffer::ReadWrite);

	format.setSampleRate(16000);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

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
}

void AudioManager::handleStateChanged(QAudio::State newState)
{
	switch (newState) {
		case QAudio::IdleState:
			// Finished playing (no more data)
			//audioOut->stop();
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


void AudioManager::play()
{
	audioOut->start(audioBuffer);
}
void AudioManager::stop()
{
	audioOut->stop();
	audioBuffer->seek(0);
	audioBuffer->close();
}
