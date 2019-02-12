#pragma once

#include "deps/QTFFmpegWrapper/ffmpeg.h"
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDebug>

class VideoStitcher : public QObject {
	Q_OBJECT

signals:
	void finished();

public:
	void start();
	void addFile(QString filePath);
	bool isFileAdded(QString filePath);
	
private slots:
	void processFinished(int, QProcess::ExitStatus);

private:

	QStringList originalFiles;

	AVFormatContext *input_format_context;
	AVStream *input_video_stream;

	AVFormatContext *output_format_context;
	AVStream *output_video_stream;

	QString outputPathTemp();
	QString outputPath();
	QString filesTxt();
	
	void eraseOriginalFiles();
	void concatenateFilesInto(QString);
};
