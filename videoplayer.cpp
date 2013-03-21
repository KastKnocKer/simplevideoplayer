#include "videoplayer.h"

//COSTRUTTORE
videoplayer::videoplayer(QWidget *parent)
{

	createMenu();	

    //includendo gli oggetti menu, bottoni e frame video in QVBoxLayout che li ordina verticalmente
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->setMenuBar(menuBar);

    // creo il display LDC e la barra di scorrimento video

    // LCD
    QPalette palette;
    palette.setBrush(QPalette::Light, Qt::darkGray);

    timeLcd = new QLCDNumber;
    timeLcd->setPalette(palette);

    //------------------------------------------------------------------
    /*qui devo gestire l'lcd con il video
    mediaObject->setTickInterval(1000);
    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));*/
    //-------------------------------------------------------------------

    //Barra di scorrimento
    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 0);

    //Inglobo la barra e LCD in un contenitore che li mette io orizzontale
    QHBoxLayout *seekerLayout = new QHBoxLayout;
    seekerLayout->addWidget(positionSlider);
    seekerLayout->addWidget(timeLcd);
    //aggiungo il contenitore alla finestra principale
    mainLayout->addLayout(seekerLayout);

    //Ora mi occupo dei pulsati
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    playAction->setShortcut(tr("Ctrl+P"));
    playAction->setDisabled(true);
    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    pauseAction->setShortcut(tr("Ctrl+A"));
    pauseAction->setDisabled(false);
    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    stopAction->setShortcut(tr("Ctrl+S"));
    stopAction->setDisabled(false);

	//event listener dei pulsanti
	QObject::connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()) );
	
    QToolBar *bar = new QToolBar;
    bar->addAction(playAction);
    bar->addAction(pauseAction);
    bar->addAction(stopAction);

    QLabel *volumeLabel = new QLabel;
    volumeLabel->setPixmap(QPixmap(":/images/volume.png"));

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0,100);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    //aggiungo i bottoni alla finestra principale
    QHBoxLayout *controlButton = new QHBoxLayout;
    controlButton->addWidget(bar);
    controlButton->addStretch();
    controlButton->addWidget(volumeLabel);
    controlButton->addWidget(volumeSlider);
    controlButton->addSpacing(0);// questa serve per spostare il volume a destra e di quanto dal bordo
    mainLayout->addLayout(controlButton);


    setLayout(mainLayout);

    // setto il titolo del finestra
    setWindowTitle(tr("VideoPlayer"));

    setFixedSize(400,100);

	//ogni volta che dal clock viene richiesto un update della finestra, richiamo lo slot updateGL
	//connect(_clock, &AVClock2::needupdate, &window, &Video::updateGL);
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
    msgbox.about(this, tr("Info MediaPlayer"),tr("<center><b>Mediaplayer</b> è stato implementato utilizzando Qt 4.8, openGL e ffmpeg realizzato da Gagliardelli Luca, Renzi Matteo e Esposito Giovanni</center>"));
 }

/**
funzione per il timer digitale
*/
 void videoplayer::tick(qint64 time)
 {
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

     timeLcd->display(displayTime.toString("mm:ss"));
 }

 /**
 metodo che ritorna il path del video selezionato
 */

 std::string videoplayer::getSourceFilename(){

	 return _fileName.toStdString();
 }

 /**
 metodo richiamato dall'event listener del bottone STOP
 */
 void videoplayer::stop(){

	qDebug() << "stop button pressed";
	//manager->setStopValue(1);	//imposto il valore di stop alla classe utility
 }

 void videoplayer::loadFile(){

	 /* genero un nuovo oggetto VideoState */
	is = VideoState();	

	_clock = new AVClock2();										//inizializzazione del clock
	_clock->SetVideoState(&is);										//gli passo il puntatore a VideoState

	is.setSourceFilename(this->getSourceFilename());				//imposto il nome del file all'oggetto VideoState
	is.window = &window;

	window.show();

	// Register all formats and codecs
	av_register_all();
	
	//inizializzazione di SDL audio interface
	if(initializeSDL() == -1){
		qDebug() << "error initializing SDL AUDIO";
		exit(1);
	}

	/* mando in esecuzione il primo refresh, gli altri avverranno in cascata, grazie alla connect */
	_clock->schedule_refresh(40);									

	/* lancio il thread di decodifica generale, apre i rispettivi codec,
	legge stream x stream e genera paccehtti audio e video che eseguiranno i distinti thread
	*/
	_demuxer = new DecodeThread();									//inizializzo oggetto DecodeThread
	_demuxer->SetVideoState(&is);
	_demuxer->set(_demuxer);										//setto puntatore statico all'oggetto
	is.parse_tid = _demuxer;
	_demuxer->start();

	//nota: VideoState di default dovrebbe avere un riferimento al thread....
	if(!is.parse_tid) {
		qDebug() << "ERRORE: riferimento al thread di decompressione mancante";
		exit(1);
	}


	//TODO devo mettere una wait? in modo che lasci scorrere la riproduzione?
	//ho tolto la l'ascoltatore di SDL event

	return;

 }

 //////////////////////////////////////////////////////////////////////////////////////////////////

/**
inizializzaizone della di SDL AUDIO
*/
int videoplayer::initializeSDL(){

	if(SDL_Init(SDL_INIT_AUDIO)) {							//Inizializzo SDL per visualizzazione
		qDebug() << "Impossibile inizializzare le SDL per AUDIO " << SDL_GetError();
		return -1;
	}

	return 0;
}