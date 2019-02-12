#include "video-trimmer.hpp"
#include <QFile>
#include <QMessageBox>
#ifdef _WIN32
#define FFMPEG_EXECUTABLE "ffmpeg.exe"
#else
#define FFMPEG_EXECUTABLE "./ffmpeg"
#endif

VideoTrimmer::VideoTrimmer(QString inputFile, int beginning, int end) {
	this->inputFile = inputFile;
	this->beginning = beginning;
	this->end = end;
}


QString VideoTrimmer::tempFile() {
	return QString(inputFile).replace(".mp4", "-temp.mp4");
}

QString VideoTrimmer::timecode(int milliseconds) {
	int seconds = (milliseconds / 1000) % 60;
	int minutes = ((milliseconds / 1000) / 60) % 60;
	int hours = ((milliseconds / 1000) / 60) / 60;
	int frames = (milliseconds % 1000) / 33;
	return QString("%1:%2:%3.%4")//skyli changed "%1:%2:%3.%4"
		.arg(QString::number(hours), 2, QChar('0'))
		.arg(QString::number(minutes), 2, QChar('0'))
		.arg(QString::number(seconds), 2, QChar('0'))//skyli changed
		.arg(QString::number(frames), 3, QChar('0'));
}


void VideoTrimmer::start() {
	int duration = end - beginning;

	QStringList arguments;
	//ffmpeg -i yourvideoname.mp4 -ss 00:00:45 -codec copy -t 40 outputclip.mp4 ///*timecode(duration)*/skyli changed
	//do not use -codec copy or -c copy
	QFile *temp = new QFile(tempFile());
	if (temp->exists())
	{
		temp->remove();
	}
	arguments << "-ss" << timecode(beginning) << "-i" << inputFile << "-t" << timecode(duration) << tempFile();
	QProcess *process = new QProcess(this);
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
	//skyli added
	auto processStarted = []
	{
		QMessageBox msgBox;
		msgBox.setText("Error! Process can not start.");
		msgBox.exec();
	};
	connect(process, &QProcess::errorOccurred, processStarted);
	//you sould have ffmpeg.exe in the same directory with the scap.exe
	process->start(FFMPEG_EXECUTABLE, arguments);
	qDebug() << "QProcess started" << timecode(beginning) << timecode(duration) << tempFile();
}

void VideoTrimmer::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	QFile(inputFile).remove();
	QFile *temp = new QFile(tempFile());
	temp->rename(inputFile);
	qDebug() << "QProcess finished" << exitCode << exitStatus;
	emit finished();
}