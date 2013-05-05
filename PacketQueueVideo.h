#pragma once

//QT
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

//STD
#include <list>

//altro
#include "Status.h"
#include "StaticValue.h"

class PacketQueueVideo
{

private:

	std::list<AVPacket> queue;

	QMutex			*_mutex;
	QWaitCondition  *_cond;

	Status *ut;

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

	/**
	ritorna la dimensione della lista (numero pacchetti)
	*/
	inline int getSize();

	QMutex* getMutex();
	QWaitCondition* getCond();

	void setUtility(Status *ut);
};

//////////////////////////////////////////////////////////////////////
// METODI INLINE

/**
ritorna la dimensione della lista
*/
int PacketQueueVideo::getSize(){

	return queue.size();

};
