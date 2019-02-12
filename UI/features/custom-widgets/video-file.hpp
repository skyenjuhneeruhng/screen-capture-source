#pragma once

#include "deps/QTFFmpegWrapper/QVideoDecoder.h"
#include <QImage>

class VideoFile
{

public:
	VideoFile(QString);
	~VideoFile();
	
	QImage firstFrame();
	QImage frameAt(int);
	QImage frame(int64_t);

private:
	QVideoDecoder decoder;
};
