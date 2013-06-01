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
#include <QIcon>

#include "DecodeThread.h"
#include "VideoState.h"
#include "AVClock.h"
#include "HistoThread.h"
#include "Log.h"



class QMenu;
class QMenuBar;
class QPushButton;
class QLCDNumber;
class glwidget;
class QSlider;
class QThread;
class QSignalMapper;
class QTime;
class QIcon;

/*
	Classe che implementa l'interfaccia grafica
	@author Giovanni Esposito
*/
class videoplayer : public QWidget
{
	Q_OBJECT

private:

	QString _fileName;			/* Nome del file video */

    QMenuBar *menuBar;			/* Barra che contiene i pulsanti */
    QMenu *fileMenu;			/* Menù file */
    QMenu *About;				/* Menù about */
    QAction *actionOpen;		/* Azione apri */
    QAction *exitAction;		/* Azione esci */
    QAction *actionInfo;		/* Azione info */
    QLCDNumber *panelLCD;		/* Pannello LCD che mostra avanzamento video */
    QSlider *positionSlider;	/* Barra che mostra avanzamento video */
    QSlider *volumeSlider;		/* Regolazione volume */
    QAction *playAction;		/* Pulsante play */
    QAction *pauseAction;		/* Pulsante pausa */
    QAction *stopAction;		/* Pulsante stop */
	QAction *skipforwardAction;	/* Pulsante avanti */
	QAction *skipbackwardAction;/* Pulsante indietro */
	QAction *seekforwardAction;	/* Seek avanti */
	QAction *seekbackwardAction;/* Seek indietro */
	QSignalMapper *signalMapper;/* Fa da mapping per particolari signal */
	DecodeThread *_demuxer;		/* Puntatore al thread di decodifica */
	AVClock *_clock;			/* Puntatore alla classe di refresh */
	bool stoptick;				/* Variabile per evitare aggiornamento dello slider */
	int64_t time;				/*  */
	HistoThread *_histo;			/* Thread dell'istogramma */
	QAction *histoAction;		/* bottone per attivare/disattivare istogramma */
	QAction *logAction;
	Log *_logDialog;

	/**
		Crea il menù
	*/
	void createMenu();

	/**
		Effettua lo stop del video
	*/
	void stop();

	/**
		Inizializzazione di SDL per audio
		@return -1 in caso di errore, altrimenti 0
	*/
	int initializeSDL();

	/**
		Metodo che va a impostare le flag di seek
		@param pos
		@param rel
	*/
	void stream_seek(int64_t pos, int64_t rel);


protected:

	/**
		Ridefinizione dell'evento di chiusura della finestra
		@param event evento chiusura finestra
	*/
	void closeEvent(QCloseEvent *event);

signals:

	/**
	
	*/
	void first_play();
	

public slots:
	/**
		Apertura video
	*/
    void open(void);

	/**
		Finestra about
	*/
    void about(void);
    
	/**
		Metodo che dopo che ho scelto il file fa partire la riproduzione
	*/
	void loadFile();

	/**
		Metodo per fermare la riproduzione
	*/
	void quit();

	/**
		Metodo per mettere in pausa la riproduzione
	*/
	void pause();

	/**
		Richiama playing e fa ripartire audio
	*/
	void resume();

	/**
		metodo usato per abilitare/disabilitare i pulsanti durante la riproduzione
	*/
	void playing();

	//SLIDER E LCD
	
	/**
		Metodo per richiamare il seek
		@param incr tempo in millisecondi
	*/
	void seek(int incr);

	/**
		Metodo di refresh del timer
	*/
	inline void tick();
	
	/**
		Metodo per resettare a 0 slider e LCD
	*/
	void resetSlider();
	
	/**
		A differenza di seek io non ho un incremento ma un nuovo tempo
	*/
	void slider_seek();
	
	/**
		metodo per fermare il refresh dello slider
	*/
	inline void stop_tick();

	/**
		metodo per gestire la la visualizzazione o meno istogramma
	*/
	void histoClicked();

	/**
		Metodo per aprire il dialog contenente le informazioni
		di riproduzione del video
	*/
	void openDialog();

public:

	Video *window;		/* Finestra di riproduzione */
	HistoDraw *histo_window;
	VideoState is;		/* Classe contente tutti i dati sul video in riproduzione */

	/**
		Costruttore
		@param parent
	*/
	videoplayer(QWidget *parent = 0);
	
	/**
		Distruttore
	*/
	~videoplayer();

	/**
		Metodo per ottenere il nome del file selezionato
		@return Ritorna il nome del file selezionato
	*/
	std::string getSourceFilename();

};

#endif // VIDEOPLAYER_H


///////////////////////////////////////////////////////////////////////////////////////////////////
// INLINE


/**
	SLOT: funzione per aggiornamento del timer digitale e dello slider
*/
void videoplayer::tick()
{
	 if(stoptick){
		 return;
	 }

	 time = (int64_t) (_clock->get_master_clock()+0.5);
	 positionSlider->setValue((int) time);

	 //QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
	 QTime displayTime(0, (time / 60) % 60, (time) % 60);
     panelLCD->display(displayTime.toString("mm:ss"));
}

/**
	SLOT: quando premo sullo slider, devo evitare aggiornamento di essa
*/
void videoplayer::stop_tick()
{
	stoptick = true;
}