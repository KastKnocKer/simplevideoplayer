#pragma once

//QT
#include <QDebug>
#include "SDL\include\SDL.h"
#include "SDL\include\SDL_thread.h"

//STD
#include <list>

extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libavformat\avio.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
	#undef main
}

//altro
#include "StaticValue.h"

class PacketQueueAudio
{

private:

	std::list<AVPacket> queue;

	SDL_mutex	*_mutex;
	SDL_cond *_cond;

	AVPacket *flush_pkt;

	int *_quit;

public:

	PacketQueueAudio(void);
	~PacketQueueAudio(void);

	int Get(AVPacket* pkt, int block);
	int Put(AVPacket* pkt);

	/**
	metodo per svuotare la lista
	*/
	int Flush();

	
	void quit();

	/**
	ritorna la dimensione della lista (numero pacchetti)
	*/
	int getSize();

	SDL_mutex* getMutex();
	SDL_cond* getCond();

	/**
	metodo per settare il riferimento al pacchetto di FLUSH
	*/
	void setFlushPkt(AVPacket *pkt);

	void setQuitVariable(int *quit);
};
