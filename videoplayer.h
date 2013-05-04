#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <string>
#include <iostream>
#include <thread>
#include <inttypes.h>

#include <QDialog>
#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include <QSignalMapper>
#include <QTime>

#include "DecodeThread.h"
#include "VideoState.h"
#include "AVClock2.h"



class QMenu;
class QMenuBar;
class QPushButton;
class QLCDNumber;
class glwidget;
class QSlider;
class QThread;
class QSignalMapper;
class QTime;

class videoplayer : public QWidget
{
	Q_OBJECT

private:

	bool debug;

	QString _fileName;														//nome del file video

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *About;
    QAction *actionOpen;
    QAction *exitAction;
    QAction *actionInfo;
    QLCDNumber *panelLCD;
    QSlider *positionSlider;
    QSlider *volumeSlider;
    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;
	QAction *skipforwardAction;
	QAction *skipbackwardAction;
	QAction *seekforwardAction;
	QAction *seekbackwardAction;
	QSignalMapper *signalMapper;							//fa da mapping per particolari signal

	void createMenu();
	void stop();
	int initializeSDL();									//inizializzazione di SDL per audio

	DecodeThread *_demuxer;									//puntatore al thread di decodifica
	AVClock2 *_clock;


	void stream_seek(int64_t pos, int64_t rel);

signals:

	void first_play();
	

public slots:

    void open(void);
    void about(void);
    

	void loadFile();										//metodo una volta che ho scelto il file, che fa partire la riproduzione
	void quit();											//Metodo per fermare la riproduzione
	void pause();

	void resume();											//richiama playing e fa ripartire audio
	void playing();

	//SLIDER E LCD
	/**
	metodo per richiamare il seek
	@param: tempo in millisecondi
	*/
	void seek(int incr);
	void tick();											//metodo di refresh del timer
	void resetSlider();										//metodo per resettare a 0 slider e LCD
	void slider_seek();										//a differenza di seek io non ho un incremento ma un nuovo tempo

public:

	Video *window;														//finestra di riproduzione
	VideoState is;														//classe contente tutti i dati sul video in riproduzione

	//COSTRUTTORE
	videoplayer(QWidget *parent = 0);
	//DISTRUTTORE
	~videoplayer();

	//ritorna il nome del file selezionato
	std::string getSourceFilename();

	

};

#endif // VIDEOPLAYER_H