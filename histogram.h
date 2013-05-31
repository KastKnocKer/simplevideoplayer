#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <string>
#include <vector>

//QT
#include <QThread>

#include "VideoState.h"


class QThread;

/**
	
*/
class histogram : public QThread
{
	Q_OBJECT

private:

	VideoState *_is;					/* Riferimento allo stato globale del video */

	std::pair<AVFrame*, double> cur;	/* Frame corrente */

	std::vector <int> R;				/* Occorrenze pixel rossi */
	std::vector <int> G;				/* Occorrenze pixel gialli */
	std::vector <int> B;				/* Occorrenze pixel blu */

	void run();

signals:

public:
	/**
		Costruttore
		@param parent
	*/
	histogram(QObject *parent = 0);

	/*
		Metodo per impostare il riferimento allo stato del video
		@param is stato del video
	*/
	void SetVideoState(VideoState *is);

};

#endif //histogram