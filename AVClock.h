#pragma once

#include <QTimer>

#include <math.h>

#include "VideoState.h"

class QTimer;
class QObject;

class AVClock : public QObject
{

	Q_OBJECT

public:

	typedef enum {
        AudioClock, VideoClock, ExternalClock
	} ClockType;


private:

	VideoState *_is;
	ClockType clock_type;
	AVFrame *pFrameRGB;
	QTimer *timer;			//timer per il refresh della finestra

	double actual_delay;
	double delay;
	double sync_threshold;
	double ref_clock;
	double diff;
	double frame_current_pts;

public slots:

	/**
	funzione che va ad reperire un frame dalla coda e inviarlo alla classe
	di refresh del video display, solo dopo un determinato intervallo di tempo
	*/
	void video_refresh_timer(void);

	//utilizzato per stoppare il timer e resettare le variabili
	void reset();		

signals:

	//segnale emesso dal refresh per andare a chiamare il paint della finestra
	void needupdate();
	
public:

	AVClock(QObject *parent = 0);
	~AVClock(void);

	double get_audio_clock();
	double get_video_clock();
	double get_external_clock();

	/**
	metodo che ritorna il clock, in base al tipo di sincronizzazione impostato
	*/
	double get_master_clock();

	void setClockType(ClockType ct);
	AVClock::ClockType clockType() const;

	/**
	metodo per impostare una chiamata ritardata a video_refresh_timer
	dopo un tot di millisecondi
	@param: ritardo con in quale eseguire il refresh [s]
	*/
	void schedule_refresh(int delay);

	void SetVideoState(VideoState *is);
	VideoState* GetVideoState();

	QTimer* getTimer();
};

