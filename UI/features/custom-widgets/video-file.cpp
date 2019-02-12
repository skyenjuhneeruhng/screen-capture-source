#include "video-file.hpp"
#include <QDebug>


VideoFile::VideoFile(QString filePath) {
	decoder.openFile(filePath);
}

VideoFile::~VideoFile() {
  decoder.close();
}


QImage VideoFile::frameAt(int milliseconds) {
	QImage image;
	decoder.seekMs(milliseconds);
	decoder.getFrame(image);
  return image;
}

QImage VideoFile::frame(int64_t frame) {
	QImage image;
	decoder.seekFrame(frame);
	decoder.getFrame(image);
  return image;
}


QImage VideoFile::firstFrame() {
	return frame(1);
}