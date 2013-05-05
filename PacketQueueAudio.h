#pragma once

//QT
#include <QDebug>

#include "SDL\include\SDL.h"
#include "SDL\include\SDL_thread.h"

//STD
#include <list>

//altro
#include "StaticValue.h"
#include "Status.h"

class PacketQueueAudio
{

private:

	std::list<AVPacket> queue;

	SDL_mutex	*_mutex;
	SDL_cond *_cond;

	Status *ut;

public:

	PacketQueueAudio(void);
	~PacketQueueAudio(void);

	int Get(AVPacket *pkt, int block);
	int Put(AVPacket *pkt);

	/**
	metodo per svuotare la lista
	*/
	int Flush();

	void quit();

	/**
	ritorna la dimensione della lista (numero pacchetti)
	*/
	inline int getSize();

	SDL_mutex* getMutex();
	SDL_cond* getCond();

	void setUtility(Status *ut);
};


///////////////////////////////////////////////////////////////////////
// METODI INLINE

/**
ritorna la dimensione della lista
*/
int PacketQueueAudio::getSize(){

	return queue.size();

};
