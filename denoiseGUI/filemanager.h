#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QWidget>

class FileManager : public QWidget
{
	Q_OBJECT
	public:
		explicit FileManager(QWidget *parent = 0);

	signals:

	public slots:
		void open();
		void openOriginal();
		void save();
	signals:
		void noisyLoaded(bool);
		void originalLoaded(bool);
	private:
		bool loaded;
};

#endif // FILEMANAGER_H
