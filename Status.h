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

	#undef main
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

	inline bool isPlaying();

	void setStopValue(bool quit);
	inline bool getStopValue();

	void setPauseValue(bool pause);
	inline bool getPauseValue(); 

	void setLastPauseValue(bool lastpause);
	inline bool getLastPauseValue();

	inline bool isPauseChanged();

	void setEOFValue(bool eof);
	inline bool getEOFValue();

	AVPacket* getFlushPkt();

};

////////////////////////////////////////////////////////////////////////////////
// METODI INLINE

bool Status::getStopValue(){
	return this->_quit;
};

bool Status::getPauseValue(){	
	return this->_pause;
};

bool Status::getLastPauseValue(){	
	return this->_lastpause;
};

bool Status::isPauseChanged(){
	if(_pause != _lastpause){
		return true;
	} else {
		return false;
	}
}

bool Status::getEOFValue(){
	return _eof;
}

bool Status::isPlaying(){

	if(_quit || _eof){
		return false;
	}
	else return true;
}

