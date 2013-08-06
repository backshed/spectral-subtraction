#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QtMultimedia/QAudioOutput>
#include <QBuffer>
class AudioManager : public QObject
{
		Q_OBJECT
	public:
		explicit AudioManager();

		void writeAudio(short* ext_buffer, unsigned int len);
		void play();
		void stop();


	private:
		QBuffer* audioBuffer;
		QAudioFormat format;
		QAudioOutput *audioOut;

};

#endif // AUDIOMANAGER_H
