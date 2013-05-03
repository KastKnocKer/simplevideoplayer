#pragma once

/**
classe per la gestione di parametri fondamentali per piu processi dell'applicativo
i valori sono privati e accessibili solo tramite apposite chiamate
*/
#include <QDebug>

extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libavformat\avio.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
	#undef main
}

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