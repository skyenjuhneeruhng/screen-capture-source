#pragma once

#include "deps/QTFFmpegWrapper/ffmpeg.h"
#include <QObject>
#include <QProcess>
#include <QString>
#include <QDebug>

class VideoTrimmer : public QObject {
	Q_OBJECT

public:
	VideoTrimmer(QString inputFile, int beginning, int end);
	void start();

signals:
	void finished();
	
private slots:
	void processFinished(int, QProcess::ExitStatus);

private:
	QString inputFile;
	int beginning;
	int end;
	
	QString timecode(int);
	QString tempFile();
};
