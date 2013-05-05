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


class DecodeThread : public QThread
{
	Q_OBJECT

private:

	AVClock* _clock;
	VideoState *_is;
	
	AVPacket _pkt1, *_packet;

	AVFormatContext *_pFormatCtx;

	int video_index;
	int audio_index;
	
	AVDictionary *io_dict;

	VideoThread *_video_th;

	int duration;

	/*
	corpo del thread,
	viene inizializzato FFMPEG
	suddivisione dello stream in pacchetti e distinzione audio o video
	*/
	void run();

signals:

	//per settare il range dello slider
	void setSliderRange(int start, int end);
	void eof();

public:

	DecodeThread(QObject *parent = 0);

	/* questo metodo e usato sia per lo stream audio che video, ed esegue:
	- apertura codec
	- setting codec a VideoState
	- [nel caso audio apertura di SDL_OpenAudio]
	- inizializzazione delle rispettive queue
	- inizializzazione del rispettivo thread di riproduzione audio/video
	*/
	int stream_component_open(int stream_index);

	/**
	metodo per settare la variabile globale decompressionThread
	*/
	void set(DecodeThread *t);

	void SetVideoState(VideoState *is);
	VideoState* GetVideoState();

	AVClock* GetAVClock();
	void SetAVClock(AVClock *c);

	void fail(void);

};

#endif