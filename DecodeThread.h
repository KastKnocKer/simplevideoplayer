#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <string>

#include <QDebug>

#include "AudioManager.h"
#include "VideoThread.h"
#include "AVClock.h"

//definisco una costante che rapresenta il formato dei pixel voluto commento perché è già definita in videothread
//const PixelFormat CONV_FORMAT = PIX_FMT_RGB24;	

class QThread;

/**
	Classe che si occupa di estrarre dal file i pacchetti audio e video
*/
class DecodeThread : public QThread
{
	Q_OBJECT

private:

	AVClock* _clock;					/* */
	VideoState *_is;					/* Puntatore allo stato globale delvideo */
	
	AVPacket _pkt1;						/* */
	AVPacket *_packet;					/* */

	AVFormatContext *_pFormatCtx;		/* */

	int video_index;					/* */
	int audio_index;					/* */
	
	AVDictionary *io_dict;				/* */

	VideoThread *_video_th;				/* */

	int duration;						/* */

	/**
		Corpo del thread.
		In questo metodo viene inizializzato FFMPEG e viene effettuata la
		suddivisione dello stream in pacchetti e la distinzione audio o video
	*/
	void run();

signals:

	/**
		Per settare il range dello slider
		@param start
		@param end
	*/
	void setSliderRange(int start, int end);

	/**
	
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
		@return
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
		
	*/
	void fail(void);

};

#endif