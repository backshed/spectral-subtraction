#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#ifndef __arm__
#include <QtMultimedia/QAudioOutput>
#else
#include <QtMultimediaKit/QAudioOutput>
#endif

#include <QBuffer>
class AudioManager : public QObject
{
		Q_OBJECT
	public:
		explicit AudioManager();

		void writeAudio(short* ext_buffer, unsigned int len);
		void play();
		void stop();

	public slots:
		void handleStateChanged(QAudio::State newState);

	private:
		QBuffer* audioBuffer;
		QAudioFormat format;
		QAudioOutput *audioOut;

};

#endif // AUDIOMANAGER_H
