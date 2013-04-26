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

	AVPacket *flush_pkt;

	int *_quit;
	int *_eof;

public:

	PacketQueueVideo(void);
	~PacketQueueVideo(void);

	int Get(AVPacket *pkt, int block);
	int Put(AVPacket *pkt);

	/**
	metodo per svuotare la lista
	*/
	int Flush();

	void quit();

	//ritorna la dimensione della lista
	int GetSize();

	QMutex* getMutex();
	QWaitCondition* GetCond();

	/**
	metodo per settare il riferimento al pacchetto di FLUSH
	*/
	void setFlushPkt(AVPacket *pkt);

	void setQuitVariable(int *quit);

	void setEOFVariabile(int *eof);
};
