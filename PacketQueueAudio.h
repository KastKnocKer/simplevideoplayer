#pragma once

//QT
#include <QDebug>
#include <SDL.h>
#include <SDL_thread.h>

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

class PacketQueueAudio
{

private:

	std::list<AVPacket> queue;

	SDL_mutex	*_mutex;
	SDL_cond *_cond;

	Utility *manager;

public:

	PacketQueueAudio(void);
	PacketQueueAudio(Utility *manager);
	~PacketQueueAudio(void);

	int Get(AVPacket* pkt, int block);
	int Put(AVPacket* pkt);
	void quit();

	//ritorna la dimensione della lista
	int getSize();

	SDL_mutex* getMutex();
	SDL_cond* getCond();

};
