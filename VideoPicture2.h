#ifndef VIDEOPICTURE_H
#define VIDEOPICTURE_H


//FFMPEG
extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
	#undef main
}

#include <list>
#include <utility>

#include <QMutex>
#include <QWaitcondition>

#include "StaticValue.h"

/**
classe che mantiene la coda dei frame gia convertiti in RGB e del rispetto
PTS (suo o assegnato)
*/
class VideoPicture2
{
private:

	QWaitCondition	*_cond_maxsize, *_cond_data;
	QMutex			*_mutex_maxsize, *_mutex_data;

	//std::list<AVFrame> queue;
	std::list<std::pair<AVFrame*, double>> queue;

	//TEMPORANEAMENTE STATICO
	int quit;

public:
	VideoPicture2(void);

	~VideoPicture2(void);

	int Put(AVFrame *pFrameRGB, double pts);

	std::pair<AVFrame*, double> Get();

	int getSize();
};

#endif //VIDEOPICTURE