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
class VideoPicture
{
private:

	QWaitCondition	*_cond_maxsize, *_cond_data;
	QMutex			*_mutex_maxsize, *_mutex_data;

	//std::list<AVFrame> queue;
	std::list<std::pair<AVFrame*, double>> queue;

	Status *ut;

public:

	VideoPicture(void);

	~VideoPicture(void);

	int Put(AVFrame *pFrameRGB, double pts);

	std::pair<AVFrame*, double> Get();

	void Flush();

	int getSize();

	void setUtility(Status *ut);
};
