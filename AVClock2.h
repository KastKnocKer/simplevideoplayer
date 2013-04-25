#pragma once

#include <QTimer>
#include <QElapsedTimer>

#include <math.h>

#include "VideoState.h"

class QTimer;
class QObject;

class AVClock2 : public QObject
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

public slots:

	void video_refresh_timer(void);
	//utilizzato per stoppare il timer e resettare le variabili
	void reset();		

signals:

	void needupdate();
	void updateslider(int framenumber);
	
public:

	QTimer *timer;			//timer per il refresh della finestra

	AVClock2(QObject *parent = 0);
	~AVClock2(void);

	double get_audio_clock();
	double get_video_clock();
	double get_external_clock();

	/**
	metodo che ritorna il clock, in base al tipo di sincronizzazione impostato
	*/
	double get_master_clock();

	void setClockType(ClockType ct);
	AVClock2::ClockType clockType() const;

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

