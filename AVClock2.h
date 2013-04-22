#pragma once

#include <QTimer>
#include <QElapsedTimer>

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

	void slider_update();

signals:

	void needupdate();
	void setslider(int val);
	
public:

	QTimer *timer;			//timer per il refresh della finestra
	QTimer *sliderTimer;	//timer per il refresh della barra scorrimento

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
	@param: ritardo con in quale eseguire il refresh
	*/
	void schedule_refresh(int delay);

	void start_slider();

	void SetVideoState(VideoState *is);
	VideoState* GetVideoState();

	QTimer* getTimer();
};

