#include "videoplayer.h"

//COSTRUTTORE
videoplayer::videoplayer(QWidget *parent)
{
	stoptick = false;

	signalMapper = new QSignalMapper(this);

	createMenu();	

    //includendo gli oggetti menu, bottoni e frame video in QVBoxLayout che li ordina verticalmente
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->setMenuBar(menuBar);

    // creo il display LDC e la barra di scorrimento video

    // LCD
    QPalette palette;
    palette.setBrush(QPalette::Light, Qt::darkGray);

	//timerLCD = new QTimer(this);
    panelLCD = new QLCDNumber;
    panelLCD->setPalette(palette);

    //------------------------------------------------------------------
    //qui devo gestire l'lcd con il video
	//impongo che ogni secondo venga refreshato il pannello
	//connect(timerLCD, &QTimer::timeout, this, &videoplayer::tick);
    //-------------------------------------------------------------------

    //Barra di scorrimento
    positionSlider = new QSlider(Qt::Horizontal);

    //Inglobo la barra e LCD in un contenitore che li mette io orizzontale
    QHBoxLayout *seekerLayout = new QHBoxLayout;
    seekerLayout->addWidget(positionSlider);
    seekerLayout->addWidget(panelLCD);
    //aggiungo il contenitore alla finestra principale
    mainLayout->addLayout(seekerLayout);

    //Ora mi occupo dei pulsati
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    playAction->setShortcut(tr("Ctrl+P"));
    playAction->setDisabled(true);
    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    pauseAction->setShortcut(tr("Ctrl+A"));
    pauseAction->setDisabled(true);
    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    stopAction->setShortcut(tr("Ctrl+S"));
    stopAction->setDisabled(true);
	skipforwardAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("SkipForward"), this);
    skipforwardAction->setShortcut(tr("Ctrl+S+F"));
    skipforwardAction->setDisabled(true);
	skipbackwardAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("SkipBackward"), this);
    skipbackwardAction->setShortcut(tr("Ctrl+S+B"));
    skipbackwardAction->setDisabled(true);
	seekforwardAction = new QAction(style()->standardIcon(QStyle::SP_MediaSeekForward), tr("SeekForward"), this);
    seekforwardAction->setShortcut(tr("Ctrl+F"));
    seekforwardAction->setDisabled(true);
	seekbackwardAction = new QAction(style()->standardIcon(QStyle::SP_MediaSeekBackward), tr("SeekBackward"), this);
    seekbackwardAction->setShortcut(tr("Ctrl+B"));
    seekbackwardAction->setDisabled(true);

	histoAction = new QAction(QIcon(":/images/volume.png"), tr("histo"), this);
	logAction = new QAction(QIcon(":/images/volume.png"), tr("log"), this);


	//event listener dei pulsanti
	connect(playAction, &QAction::triggered, this, &videoplayer::resume);
	connect(this, &videoplayer::first_play, this, &videoplayer::playing);
	connect(pauseAction, &QAction::triggered, this, &videoplayer::pause);
	connect(histoAction, &QAction::triggered, this, &videoplayer::histoClicked);
	connect(logAction, &QAction::triggered, this, &videoplayer::openDialog);

	/**
	utilizzo di un signalMapper per collegare l'evento di pressione dei pulsanti SEEK,
	con un particolare valore che verra inviato allo SLOT seek
	*/
	signalMapper->setMapping(seekforwardAction, 10);
	signalMapper->setMapping(seekbackwardAction, -10);
	signalMapper->setMapping(skipbackwardAction, -60);
	signalMapper->setMapping(skipforwardAction, 60);

	connect(skipforwardAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
	connect(seekforwardAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
	connect(skipbackwardAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
	connect(seekbackwardAction, SIGNAL(triggered()), signalMapper, SLOT(map()));

	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(seek(int)));

    QToolBar *bar = new QToolBar;
    bar->addAction(playAction);
    bar->addAction(pauseAction);
    bar->addAction(stopAction);
	bar->addAction(skipbackwardAction);
	bar->addAction(seekbackwardAction);
	bar->addAction(seekforwardAction);
	bar->addAction(skipforwardAction);
	bar->addAction(histoAction);
	bar->addAction(logAction);

    QLabel *volumeLabel = new QLabel;
    volumeLabel->setPixmap(QPixmap(":/images/volume.png"));

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0,100);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    //aggiungo i bottoni alla finestra principale
    QHBoxLayout *controlButton = new QHBoxLayout;
    controlButton->addWidget(bar);
    controlButton->addStretch();
    //controlButton->addWidget(volumeLabel);
    //controlButton->addWidget(volumeSlider);
    //controlButton->addSpacing(0);// questa serve per spostare il volume a destra e di quanto dal bordo
    mainLayout->addLayout(controlButton);


    setLayout(mainLayout);

    // setto il titolo del finestra
    setWindowTitle(tr("VideoPlayer"));

    setFixedSize(400,100);

	_clock = new AVClock();										//inizializzazione del clock

	//connect sullo SLIDER
	connect(_clock, &AVClock::needupdate, this, &videoplayer::tick);
	connect(positionSlider, &QSlider::sliderReleased, this, &videoplayer::slider_seek);
	connect(positionSlider, &QSlider::sliderPressed, this, &videoplayer::stop_tick);
}

//DISTRUTTORE
videoplayer::~videoplayer()
{

}

/**
funzione grafica che permette di creare la menubar e i suoi sottomenu
*/
 void videoplayer::createMenu()
 {
     menuBar = new QMenuBar;

     fileMenu = new QMenu(tr("&File"), this);
     About = new QMenu(tr("&?"), this);
     actionInfo=About->addAction(tr("Info"));
     actionOpen= fileMenu->addAction(tr("Open"));
     fileMenu->addSeparator();
     exitAction = fileMenu->addAction(tr("Exit"));
     menuBar->addMenu(fileMenu);
     menuBar->addMenu(About);

     // collego alle voci del menu le azioni: aprire il file, uscita e info
     connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
     // close() è uno slots che eredito dalla classe QWidget quindi non la definisco
     connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
     // collego about() la funzione che crea la finestra di informazioni sul player
     connect(actionInfo, SIGNAL(triggered()), this, SLOT(about()));
 }

 /**
 funzione che apre il file-explorer per la scelta del video da riprodurre
 */
 void videoplayer::open(void)
 {
     //carico il file video, ottengo il nome del file selezionato
     _fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"" ,tr("(*.avi *.mpeg *.flv *.mp4 *.flv)"));	// per aggiungere altri formati aggiungere es. a *.avi *.mpeg *.flv
     if (!_fileName.isEmpty()){
		loadFile();																// richiamo la funione di apertura del video																
     };
 }

 /**
 funzione che apre un dialog informativo
 */
 void videoplayer::about(void)
 {
    QMessageBox msgbox;
    msgbox.about(this, tr("Info MediaPlayer"),tr("<center><b>Mediaplayer</b> implementato"
    " utilizzando Qt5, openGL e ffmpeg realizzato da Gagliardelli Luca, Renzi Matteo e Esposito Giovanni</center>"));
 }

 /**
 metodo che ritorna il path del video selezionato
 */

std::string videoplayer::getSourceFilename(){

	 return _fileName.toStdString();
 }

/**
	Metodo per aprire il dialog contenente le informazioni
	di riproduzione del video
*/
void videoplayer::openDialog(){

	_logDialog = new Log();
	_logDialog->show();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// SEEK

 /**
 SLOT: funzione per settare i parametri di seek, dal valore ottenuto dallo slider
 */
 void videoplayer::slider_seek(){

	double pos = positionSlider->value();
	double incr = pos - _clock->get_master_clock();

	//mentre passo il nuovo tempo, lo converto da secondi a microsecondi
	//(che e unita di avcodec -> timebase)
	stream_seek((int64_t) (pos*AV_TIME_BASE), (int64_t) (incr*AV_TIME_BASE));

	stoptick = false;
 }

 /**
 SLOT chiamata in seguito alla pressione di uno dei tasti di SEEK
 */
void videoplayer::seek(int incr){

	 /**
	 vado a calcolare il nuovo tempo, andando a sommare (nel caso backward sottrarre)
	 il tempo di seek a quello del master clock
	 */
	 double pos = _clock->get_master_clock();
	 pos += (double) incr;	

	 //mentre passo il nuovo tempo, lo converto da secondi a microsecondi
	 //(che e unita di avcodec)
	 stream_seek((int64_t) (pos*AV_TIME_BASE), (int64_t) (incr* AV_TIME_BASE)); 
 }

/**
funzione utilizzata per impostare le variabili di seek
*/
void videoplayer::stream_seek(int64_t pos, int64_t rel){

	if(!is.seek_req){
		is.seek_pos = pos; //abbiamo convertito il tempo nell'avcodec timestamp
		is.seek_rel = rel;
		is.seek_flags = AVSEEK_FLAG_FRAME;
		is.seek_req = 1;
	}
};

 void videoplayer::resetSlider(){

	 panelLCD->display("00:00");
	 positionSlider->setValue(0);
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////
 // CAMBIO STATO DI RIPRODUZIONE

 /**
 metodo richiamato dopo che ho chiuso la finestra di riproduzione
 */
void videoplayer::stop(){

	pauseAction->setDisabled(true);
	playAction->setDisabled(true);
	stopAction->setDisabled(true);
	skipforwardAction->setDisabled(true);
	skipbackwardAction->setDisabled(true);
	seekforwardAction->setDisabled(true);
	seekbackwardAction->setDisabled(true);
	
	//is.ut.setStopValue(true);	//imposto il valore di stop alla classe utility
	
	_clock->reset();			//stoppo il timer di refresh
	this->resetSlider();		//resetto slider e LCD	
	SDL_Quit();					//chiude completamente il corrente stream audio!!
}

/**
SLOT chiamata in seguito alla pressione di QUIT
*/
void videoplayer::quit(){

	is.ut.setStopValue(true);
	is.ut.setPauseValue(false);
	/*is.audioq.quit();
	is.videoq.quit();
	SDL_Quit();*/

}

/**
SLOT per metter in pausa la riproduzione
*/
void videoplayer::pause(void){

	pauseAction->setDisabled(true);
	playAction->setDisabled(false);
	skipforwardAction->setDisabled(true);
	skipbackwardAction->setDisabled(true);
	seekforwardAction->setDisabled(true);
	seekbackwardAction->setDisabled(true);

	is.ut.setPauseValue(true);

	SDL_PauseAudio(1);
	
}

/**
SLOT utilizzato solo quando un video gia iniziato, deve riprendere la riproduzione
*/
void videoplayer::resume(){

	if(is.ut.getPauseValue() == true){
		is.frame_timer += av_gettime()/1000000.0 + is.video_current_pts_drift - is.video_current_pts;
		if(is.read_pause_return != AVERROR(ENOSYS)){
			is.video_current_pts = is.video_current_pts_drift + av_gettime()/1000000.0;
		}
		is.video_current_pts_drift = is.video_current_pts - av_gettime()/1000000.0;
	}
	is.ut.setPauseValue(false);

	this->playing();				//riabilito i corrispettivi pulsanti

	SDL_PauseAudio(0);
}

/**
SLOT, richiamata quando il video passa in riproduzione
*/
void videoplayer::playing(){

	playAction->setDisabled(true);
	pauseAction->setDisabled(false);
	stopAction->setDisabled(false);
	skipforwardAction->setDisabled(false);
	skipbackwardAction->setDisabled(false);
	seekforwardAction->setDisabled(false);
	seekbackwardAction->setDisabled(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

//HISTOGRAM

void videoplayer::histoClicked(){

	if(histo_window->isVisible()){
		histo_window->hide();
	}
	else
		histo_window->show();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void videoplayer::loadFile(){

	emit first_play();

	/* genero un nuovo oggetto VideoState */
	is = VideoState();	

	/*inizializzo la finestra */
	window = new Video();
	/* alla chiusura della finestra mando in stop */
	connect(window, &Video::windowClosing, this, &videoplayer::stop);
	/* collego il bottone stop alla rispettiva SLOT */
	connect(stopAction, &QAction::triggered, this, &videoplayer::quit);
	/* evento di chiusura della finestra da X finestra */
	connect(window, &Video::Xpressed, this, &videoplayer::quit);

	//ogni volta che dal clock viene richiesto un update della finestra, richiamo lo slot updateGL
	connect(_clock, &AVClock::needupdate, window, &Video::updateGL);

	_clock->reset();												//resetto il clock

	_clock->SetVideoState(&is);										//gli passo il puntatore a VideoState

	is.setSourceFilename(this->getSourceFilename());				//imposto il nome del file all'oggetto VideoState
	is.window = window;

	window->show();

	// Register all formats and codecs
	av_register_all();
	
	//inizializzazione di SDL audio interface
	if(initializeSDL() == -1){
		
		exit(1);
	}

	/* mando in esecuzione il primo refresh, gli altri avverranno in cascata, grazie alla connect */
	_clock->schedule_refresh(40);	

	/* inizializzazione finestra istogramma */
	histo_window = new HistoDraw();
	histo_window->setSize(640,480);
	histo_window->show();

	is.histo_window = histo_window;

	connect(_clock, &AVClock::needupdate, histo_window, &HistoDraw::updateGL);

	/* inizializzazione thread istogramma */
	_histo = new HistoThread();
	_histo->SetVideoState(&is);
	_histo->start();
	

	/* lancio il thread di decodifica generale, apre i rispettivi codec,
	legge stream x stream e genera paccehtti audio e video che eseguiranno i distinti thread
	*/
	_demuxer = new DecodeThread();									//inizializzo oggetto DecodeThread
	_demuxer->SetVideoState(&is);
	_demuxer->SetAVClock(_clock);
	_demuxer->set(_demuxer);										//setto puntatore statico all'oggetto
	is.parse_tid = _demuxer;
	connect(_demuxer, &DecodeThread::eof, window, &Video::closeWindow);
	connect(_demuxer, &DecodeThread::eof, histo_window, &HistoDraw::closeWindow);
	_demuxer->start();
	
	/**
	connect per l'aggiornamento del max valore possibile dello slider
	coincidente con la durata del video in secondi
	*/
	connect(_demuxer, &DecodeThread::setSliderRange, positionSlider, &QSlider::setRange);

	//nota: VideoState di default dovrebbe avere un riferimento al thread....
	if(!is.parse_tid) {
		
			//qDebug() << "ERRORE: riferimento al thread di decompressione mancante";
		
		exit(1);
	}

	return;

 }

 //////////////////////////////////////////////////////////////////////////////////////////////////

/**
inizializzaizone SDL AUDIO
*/
int videoplayer::initializeSDL(){

	if(SDL_Init(SDL_INIT_AUDIO)) {							//Inizializzo SDL per visualizzazione
		//qDebug() << "Impossibile inizializzare le SDL per AUDIO " << SDL_GetError();
		return -1;
	}

	return 0;
}


//ridefinizione dell'evento di chiusura della finestra
void videoplayer::closeEvent(QCloseEvent *event){
	
	if(window != NULL){
		window->close();
	}
	event->accept();
}



