#pragma once

/**
classe per la gestione di parametri fondamentali per piu processi dell'applicativo
i valori sono privati e accessibili solo tramite apposite chiamate
*/
#include <QDebug>

//FFMPEG
extern "C"	{
	#include "dev\include\libavcodec\avcodec.h"
	#include "dev\include\libavformat\avformat.h"
	#include "dev\include\libswscale\swscale.h"
	#include "dev\include\libavutil\avutil.h"
	#include "dev\include\libavutil\time.h"
}

#pragma comment(lib, "dev\\lib\\avcodec.lib")
#pragma comment(lib, "dev\\lib\\avutil.lib")
#pragma comment(lib, "dev\\lib\\avformat.lib")
#pragma comment(lib, "dev\\lib\\swscale.lib")


class QDebug;
class Status{

private:

	bool _quit;
	bool _pause;
	bool _lastpause;
	bool _eof;

	AVPacket flush_pkt;	//pacchetto di flush usato durante il seek

public:

	Status();
	~Status();

	void setStopValue(bool quit);
	bool getStopValue();

	void setPauseValue(bool pause);
	bool getPauseValue(); 

	void setLastPauseValue(bool lastpause);
	bool getLastPauseValue();

	bool isPauseChanged();

	void setEOFValue(bool eof);
	bool getEOFValue();

	AVPacket* getFlushPkt();

};