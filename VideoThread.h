#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <string>

//QT
#include <QThread>

#include "VideoState.h"

const PixelFormat CONV_FORMAT = PIX_FMT_RGB24;	//definisco una costante che rapresenta il formato dei pixel voluto

class QThread;
/**
questo thread esegue in sequenza le seguenti azioni:
- preleva/legge i pacchetti dalla video_queue
- decodifica i pacchetti in frame
- mette i frame in una queue_picture
*/
class VideoThread : public QThread
{
	Q_OBJECT

private:

	VideoState *_is;
	AVPacket pkt1;
	AVPacket *packet;
	int len1;
	int frameFinished;
	double pts;				//il pts letto dal frame corrente
	AVFrame *pFrame, *pFrameRGB;

	int bytes;				//memoria necessaria per allocare il frame convertito in RGB

	void run();

	/**
	funzione che aggiorna i PTS in modo tale che siano sincronizzati con tutto
	devo quindi occuparsi di 2 problemi:
	1) la ripetizione di uno stesso frame
	2) sincronizzazione del video all'audio
	*/
	void synchronize_video();

signals:

	/* signal che attiverà lo slot nella finestra per un update della grafica */
	void frame_pronto();

public:

	VideoThread(QObject *parent = 0);

	void SetVideoState(VideoState *is);

};

#endif //VIDEOTHREAD