#pragma once

#include <QTimer>
#include <QElapsedTimer>

#include "VideoState.h"

class AVClock2 : public QObject
{

	Q_OBJECT

private:

	
	VideoState *_is;
	double actual_delay, delay, sync_threshold, ref_clock, diff;
	AVFrame *pFrameRGB;
	double pts;

	double get_audio_clock();

public slots:

	void video_refresh_timer(void);

signals:

	void needupdate();
	
public:

	QTimer *timer;

	AVClock2(QObject *parent = 0);
	~AVClock2(void);

	/**
	metodo per impostare una chiamata ritardata a video_refresh_timer
	dopo un tot di millisecondi
	@param: ritardo con in quale eseguire il refresh
	*/
	void schedule_refresh(int delay);

	void SetVideoState(VideoState *is);

	QTimer* getTimer();
};

