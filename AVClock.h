#pragma once

#include <QTimer>

#include <math.h>

#include "VideoState.h"

class QTimer;
class QObject;

/**
	?
*/
class AVClock : public QObject{

	Q_OBJECT

public:

	typedef enum {
        AudioClock, VideoClock, ExternalClock
	} ClockType;


private:

	VideoState *_is;			/* Puntatore all'elemento che contiene tutte le informazioni sullo stato del video */
	ClockType clock_type;		/* */
	AVFrame *pFrameRGB;			/* Frame RGB */
	QTimer *timer;				/* Timer per il refresh della finestra */

	double actual_delay;		/* */
	double delay;				/* */
	double sync_threshold;		/* */
	double ref_clock;			/* */
	double diff;				/* */
	double frame_current_pts;	/* */

public slots:

	/**
		Metodo che va ad reperire un frame dalla coda e inviarlo alla classe
		di refresh del video display, solo dopo un determinato intervallo di tempo
	*/
	void video_refresh_timer(void);

	/**
		Utilizzato per stoppare il timer e resettare le variabili
	*/
	void reset();		

signals:

	/**
		Segnale emesso dal refresh per andare a chiamare il paint della finestra
	*/
	void needupdate();
	
public:
	/**
		Costruttore
		@param parent
	*/
	AVClock(QObject *parent = 0);

	/**
		Distruttore
	*/
	~AVClock(void);

	/**
		
		@return
	*/
	double get_audio_clock();

	/**
		
		@return
	*/
	double get_video_clock();

	/**
		
		@return
	*/
	double get_external_clock();

	/**
		Metodo che ritorna il clock in base al tipo di sincronizzazione impostato
		@return ritorna audio/video clock in base al tipo di sincronizzazione impostato.
	*/
	double get_master_clock();

	/**
		
		@param ct
	*/
	void setClockType(ClockType ct);

	/**
	
	*/
	AVClock::ClockType clockType() const;

	/**
		Metodo per impostare una chiamata ritardata a video_refresh_timer dopo un tot di millisecondi
		@param delay ritardo con in quale eseguire il refresh [s]
	*/
	void schedule_refresh(int delay);

	/**
		Imposta il riferimento allo stato del video

		@param is riferimento allo stato del video
	*/
	void SetVideoState(VideoState *is);

	/**
		Metodo per ottenere lo stato attuale del video.
		@return Ritorna lo stato attuale del video.
	*/
	VideoState* GetVideoState();

	/**
		Metodo per leggere il timer
		@return il timer interno alla classe.
	*/
	QTimer* getTimer();
};

