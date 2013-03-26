#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <string>
#include <iostream>
#include <thread>

#include <QDialog>
#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QtWidgets>

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

class videoplayer : public QWidget
{
	Q_OBJECT

private:

	//Utility *manager;

	QString _fileName;														//nome del file video

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *About;
    QAction *actionOpen;
    QAction *exitAction;
    QAction *actionInfo;
    QLCDNumber *timeLcd;
    QSlider *positionSlider;
    QSlider *volumeSlider;
    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;

	void createMenu();
	void stop();
	int initializeSDL();												//inizializzazione di SDL per audio

	DecodeThread *_demuxer;												//puntatore al thread di decodifica
	AVClock2 *_clock;
	

public slots:
    void open(void);
    void about(void);
    void tick(qint64 time);

	void loadFile();													//metodo una volta che ho scelto il file, che fa partire la riproduzione
	void quit();														//Metodo per fermare la riproduzione

public:

	Video window;														//finestra di riproduzione
	VideoState is;														//classe contente tutti i dati sul video in riproduzione

	//COSTRUTTORE
	videoplayer(QWidget *parent = 0);
	//DISTRUTTORE
	~videoplayer();

	//ritorna il nome del file selezionato
	std::string getSourceFilename();

	

};

#endif // VIDEOPLAYER_H