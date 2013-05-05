#pragma once

#include <list>
#include <utility>

#include <QMutex>
#include <QWaitcondition>

#include "StaticValue.h"
#include "Status.h"

/**
classe che mantiene la coda dei frame gia convertiti in RGB e del rispettivo
PTS (suo o assegnato)
*/

struct VideoPicture{
	uint8_t data;
	double pts;
};

class QMutex;
class QWaitCondition;

class FrameQueue : public QObject
{
	Q_OBJECT

private:

	QWaitCondition	*_cond;
	QMutex			*_mutex;

	//std::list<AVFrame> queue;
	std::list<VideoPicture> queue;

	uint8_t data_read;

	Status *ut;

signals:

	void show(uint8_t *data);

public:

	FrameQueue(void);

	~FrameQueue(void);

	/* vado a inserire l'ultimo frame decodificato */
	int Put(VideoPicture);

	/* vado a reperire il PTS del frame primo in coda */
	inline double tellPTS();

	/* richiedo di mandare a schermo il prima frame in coda */
	int putInWindow();

	void Flush();

	int getSize();

	void setUtility(Status *ut);
};


double FrameQueue::tellPTS(){
	
	return queue.front().pts;	
}