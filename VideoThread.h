#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <string>

//QT
#include <QThread>

#include "VideoState.h"

const PixelFormat CONV_FORMAT = PIX_FMT_RGB24;	/* Definisco una costante che rapresenta il formato dei pixel voluto */

class QThread;

/**
	Questo thread esegue in sequenza le seguenti azioni:
		- preleva/legge i pacchetti dalla video_queue
		- decodifica i pacchetti in frame
		- mette i frame in una queue_picture

	@author Matteo Renzi
*/
class VideoThread : public QThread
{
	Q_OBJECT

private:

	VideoState *_is;	/* Riferimento allo stato globale del video */
	AVPacket pkt1;		/*  */
	AVPacket *packet;	/*  */
	int len1;			/*  */
	int frameFinished;	/* indica se ho finito di decodificare un frame */
	double pts;			/* Il pts letto dal frame corrente */
	AVFrame *pFrame;	/* Frame corrente */
	AVFrame *pFrameRGB;	/* Frame RGB corrente */

	int bytes;			/* 	Memoria necessaria per allocare il frame convertito in RGB */

	void run();

	/**
		Funzione che aggiorna i PTS in modo tale che siano sincronizzati con tutto
		devo quindi occuparsi di 2 problemi:
			1) la ripetizione di uno stesso frame
			2) aggiornamento del video_clock
	*/
	void synchronize_video();

signals:

	/**
		Signal che attiverà lo slot nella finestra per un update della grafica
	*/
	void frame_pronto();

public:
	/**
		Costruttore
		@param parent
	*/
	VideoThread(QObject *parent = 0);

	/*
		Metodo per impostare il riferimento allo stato del video
		@param is stato del video
	*/
	void SetVideoState(VideoState *is);

};

#endif //VIDEOTHREAD