project(QTFFmpegWrapper)

# if(MSVC)
# 	set(QTFFmpegWrapper_PLATFORM_DEPS
# 		w32-pthreads)
# endif()

find_package(FFmpeg REQUIRED
	# COMPONENTS avcodec swscale)
	COMPONENTS avcodec avformat avfilter avdevice avutil swscale swresample)
include_directories(${FFMPEG_INCLUDE_DIRS})

set(QTFFmpegWrapper_HEADERS
	ffmpeg.h
	QVideoDecoder.h)
	
set(QTFFmpegWrapper_SOURCES
	QVideoDecoder.cpp)
	
target_link_libraries(QTFFmpegWrapper
	${FFMPEG_LIBRARIES})