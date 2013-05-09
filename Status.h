#pragma once

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
/**
	Classe per la gestione di parametri fondamentali per piu processi dell'applicativo
	i valori sono privati e accessibili solo tramite apposite chiamate
*/
class Status{

private:

	bool _quit;			/* True se si lancia il quit */
	bool _pause;		/* True se si mette in pausa */
	bool _lastpause;	/*  */
	bool _eof;			/* Diventa true se il video è finito */

	AVPacket flush_pkt;	/* Pacchetto di flush usato durante il seek */

public:

	/**
		Costruttore
	*/
	Status();

	/**
		Distruttore
	*/
	~Status();

	/**
		Metodo per sapere se il player è in funzione.
		@return true se il player sta riproducendo un video, false altrimenti.
	*/
	inline bool isPlaying();

	/**
		Metodo per settare il quit
		@quit se true causa il quit.
	*/
	void setStopValue(bool quit);

	/**
		Metodo per vedere se il player è fermo.
		@return true se il player è fermo, false altrimenti
	*/
	inline bool getStopValue();
	
	/**
		Metodo per impostare lo stato di pausa del video.
		@param pause true se si vuole mettere in pausa, false altrimenti
	*/
	void setPauseValue(bool pause);

	/**
		Metodo per vedere se il player è in pausa o meno.
		@return true se il player è in pausa, false altrimenti
	*/
	inline bool getPauseValue(); 

	/**
	
		@param lastpause
	*/
	void setLastPauseValue(bool lastpause);

	/**
		
		@return
	*/
	inline bool getLastPauseValue();

	/**
		
		@return
	*/
	inline bool isPauseChanged();

	/**
		Metodo per impostare se il video è finito o meno
		@param eof true se il video è finito, false altrimenti
	*/
	void setEOFValue(bool eof);

	/**
		Metodo per vedere se la riproduzione è terminata o meno.
		@return true se la riproduzione è terminata, false altrimenti.
	*/
	inline bool getEOFValue();

	/**
		
		@return
	*/
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

