#include "video-stitcher.hpp"
#include <QDir>

#ifdef _WIN32
	#define FFMPEG_EXECUTABLE "ffmpeg.exe"
#else
	#define FFMPEG_EXECUTABLE "./ffmpeg"
#endif

void VideoStitcher::addFile(QString filePath) {
	originalFiles.append(filePath);
}
bool VideoStitcher::isFileAdded(QString filePath) {
	if (originalFiles.indexOf(filePath) == -1)
		return false;
	else
		return true;
}

void VideoStitcher::start() {
	if (originalFiles.length() < 2) { emit finished(); return; }
	concatenateFilesInto(outputPathTemp());
}

void VideoStitcher::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	eraseOriginalFiles();
	QFile(outputPathTemp()).rename(outputPath());
	QFile(filesTxt()).remove();
	
	qDebug() << "QProcess finished" << exitCode << exitStatus;
	emit finished();
}


QString VideoStitcher::outputPathTemp() {
	return outputPath().replace(".mp4", "-temp.mp4");
}

QString VideoStitcher::outputPath() {
	return originalFiles[0];
}

QString VideoStitcher::filesTxt() {
	QString basePath = outputPath().section("/", 0, -2);
	return QString("%1%2%3").arg(basePath).arg("/").arg("files.txt").replace("/", "\\");
}


void VideoStitcher::eraseOriginalFiles() {
	for (QString filePath : originalFiles)
		QFile(filePath).remove();
}


void VideoStitcher::concatenateFilesInto(QString outputFilePath) {
  QString program = FFMPEG_EXECUTABLE;
  QStringList arguments;
  arguments << "-y" << "-f" << "concat" << "-safe" << "0" << "-i" << filesTxt() << "-c" << "copy" << outputFilePath;
	
	QFile file(filesTxt());
	file.open(QIODevice::ReadWrite);
	for (QString videoFilePath : originalFiles)
		file.write(QString("file '%1'\n").arg(videoFilePath).toUtf8());
	file.close();

  QProcess *process = new QProcess(this);
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
  process->start(program, arguments);
	qDebug() << "QProcess started" << filesTxt();
}