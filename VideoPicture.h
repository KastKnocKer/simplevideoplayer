#pragma once

#include <list>
#include <utility>

#include <QMutex>
#include <QWaitcondition>

#include "StaticValue.h"
#include "Status.h"

/**
	Classe che mantiene la coda dei frame gia convertiti in RGB e del rispettivo
	PTS (suo o assegnato)

	@author Luca Gagliardelli
*/
class VideoPicture
{
private:

	QWaitCondition	*_cond;		/* Condition per wait */
	QMutex			*_mutex;	/* Mutex per accesso esclusivo */

	std::list<std::pair<AVFrame*, double>> queue;	/* Lista per memorizzazione frame rgb e pts */

	Status *ut;					/* puntatore alla classe di stato riproduzione */

public:
	
	/**
		Costruttore
	*/
	VideoPicture(void);

	/**
		Distruttore
	*/
	~VideoPicture(void);

	/**
		Inserisce un frame con relativo pts nella lista.
		@param pFrameRGB frame RGB da inserire
		@param pts pts associato al frame RGB
		@return -1 in caso di errore, 0 se tutto ok
	*/
	int Put(AVFrame *pFrameRGB, double pts);

	/**
		Restituisce un frame RGB con il suo pts.
		@return frame RGB con relativo PTS
	*/
	std::pair<AVFrame*, double> Get();

	/**
		Svuota la lista
	*/
	void Flush();
	
	/**
		Ritorna il numero di frame presenti nella lista
		@return numero di frame presenti nella lista
	*/
	inline int getSize();

	/**
		per settare il puntatore alla classe di stato riproduzione
		@param ut
	*/
	void setUtility(Status *ut);
};


////////////////////////////////////////////////////////////////////////////
// METOOD INLINE

int VideoPicture::getSize(void){
	return queue.size();
}
