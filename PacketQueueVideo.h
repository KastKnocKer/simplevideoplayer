#pragma once

//QT
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

//STD
#include <list>

extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
	#undef main
}

//altro
#include "utility.h"
#include "StaticValue.h"

class PacketQueueVideo
{

private:

	std::list<AVPacket> queue;

	QMutex			*_mutex;
	QWaitCondition  *_cond;

	Utility *manager;

public:

	PacketQueueVideo(void);
	PacketQueueVideo(Utility *manager);
	~PacketQueueVideo(void);

	int Get(AVPacket *pkt, int block);
	int Put(AVPacket *pkt);

	void quit();

	//ritorna la dimensione della lista
	int GetSize();

	QMutex* getMutex();
	QWaitCondition* GetCond();

};
