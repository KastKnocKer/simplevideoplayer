#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <string>

#include <QDebug>

#include "AudioManager.h"
#include "VideoThread.h"
#include "AVClock.h"


class QThread;

/**
	Thread che si occupa del flusso di decodifica,
	apre i codec audio e video se li trova
	estrae dal file i pacchetti audio e video,
	manda in esecuzione 2 nuovi thread rispettivamente per lo stream Audio e Video,
	gestione eventi di SEEK, PAUSE, STOP, EOF
	@author Luca Gagliardelli
*/
class DecodeThread : public QThread
{
	Q_OBJECT

private:

	AVClock* _clock;					/* puntatore alla classe clock */
	VideoState *_is;					/* Puntatore allo stato globale del video */
	
	AVPacket _pkt1;						/* */
	AVPacket *_packet;					/* */

	AVFormatContext *_pFormatCtx;		/* */

	int video_index;					/* indice dello stream video */
	int audio_index;					/* indice dello stream audio */
	
	AVDictionary *io_dict;				/* */

	VideoThread *_video_th;				/* */

	int duration;						/* durata in secondi */

	/**
		Corpo del thread.
		In questo metodo viene inizializzato FFMPEG e viene effettuata la
		suddivisione dello stream in pacchetti e la distinzione audio o video
	*/
	void run();

signals:

	/**
		Per settare il range dello slider.
		il valore della durata viene, se presente, ottenuto dall'header,
		se no calcolato n_frame*FPS
		@param start
		@param end
	*/
	void setSliderRange(int start, int end);

	/**
		emette un segnale che abbiamo raggiunta la fine del file
	*/
	void eof();

public:
	/**
		Costruttore
		@param parent
	*/
	DecodeThread(QObject *parent = 0);

	/**
		Questo metodo e usato sia per lo stream audio che video, ed esegue:
		- apertura codec
		- setting codec a VideoState
		- [nel caso audio apertura di SDL_OpenAudio]
		- inizializzazione delle rispettive queue
		- inizializzazione del rispettivo thread di riproduzione audio/video

		@param stream_index
		@return 0 OK, -1 ERROR
	*/
	int stream_component_open(int stream_index);

	/**
		Metodo per settare la variabile globale decompressionThread

		@param t variabile globale decompressionThread
	*/
	void set(DecodeThread *t);

	/**
		Imposta il riferimento all'oggetto che mantiene lo stato del video
		@param is riferimento all'oggetto che mantiene lo stato del video
	*/
	void SetVideoState(VideoState *is);

	/**
		Metodo per ottenere lo stato del video.
		@return lo stato del video.
	*/
	VideoState* GetVideoState();

	/**
		Metodo per leggere il clock
		@return il clock
	*/
	AVClock* GetAVClock();

	/**
		Metodo per settare il clock
		@param c
	*/
	void SetAVClock(AVClock *c);

	/*
		Metodo richiamato ogni volta che finisce la riproduzione EOF o STOP o ERROR
	*/
	void fail(void);

};

#endif