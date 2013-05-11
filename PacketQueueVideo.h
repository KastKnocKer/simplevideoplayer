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

/*
	Classe che implementa la coda per la gestione
	dei pacchetti video. Accesso alla coda mutuamente esclusivo.

	@author Luca Gagliardelli
*/
class PacketQueueVideo{

private:

	std::list<AVPacket> queue;	/* Coda in cui vengono memorizzati i pacchetti */

	QMutex			*_mutex;	/* Mutex qt per consentire un accesso esclusivo alla coda */
	QWaitCondition  *_cond;		/* Coda SDL per mettere in wait */

	Status *ut;					/*  */

public:
	
	/**
		Costruttore
	*/
	PacketQueueVideo(void);

	/**
		Distruttore
	*/
	~PacketQueueVideo(void);

	/**
		Metodo per prelevare un pacchetto dalla coda.
		@param pkt puntatore al pacchetto su cui scrivere il pacchetto prelevato
		@param block se block è 1 e la coda è vuota va in wait, altrimenti esce con valore di ritorno 0
		@return ritorna -1 in caso di errore, 0 se non preleva nulla perché la coda è vuota, 1 in caso di pacchetto prelevato correttamente.
	*/
	int Get(AVPacket *pkt, int block);

	/**
		Metodo per inserire un pacchetto audio all'interno della coda.
		@param pkt pacchetto da inserire nella coda
		@return ritorna -1 in caso di errore, altrimenti 0.
	*/
	int Put(AVPacket *pkt);

	/**
		Metodo per svuotare la lista
		@return ritorna zero.
	*/
	int Flush();

	/**
		Metodo da lanciare in caso di chiusura.
		Svuota la lista e sveglia eventuali processi in wait.
	*/
	void quit();

	/**
		Metodo per ottenere la dimensione della lista (numero pacchetti)
		@return Ritorna la dimensione della lista (numero pacchetti)
	*/
	inline int getSize();

	/*
		Metodo per ottenere il mutex.
		@return il mutex usato nella classe
	*/
	QMutex* getMutex();

	/*
		Metodo per ottenere la condition
		@return la condition usata nella classe
	*/
	QWaitCondition* getCond();

	/*
		
		@param ut
	*/
	void setUtility(Status *ut);
};

//////////////////////////////////////////////////////////////////////
// METODI INLINE

/**
	Ritorna la dimensione della lista
*/
int PacketQueueVideo::getSize(){

	return queue.size();

};
