#include "GUIThread.h"

GUIThread::GUIThread(int fullscreen)
{
	_fullscreen = fullscreen;
}


/*
	codice eseguito dal thread
*/
void GUIThread::run(void){

	SDL_Event	event;

	/* genero un nuovo oggetto VideoState */
	is = VideoState();		

	is.setSourceFilename(_fileName);				//imposto il nome del file all'oggetto VideoState

	// Register all formats and codecs
	av_register_all();
	
	if(initializeSDL() == -1){
		qDebug() << "error initializing SDL Window";
		exit(1);
	}

	_clock = new AVClock(AVClock::ExternalClock);		//inizializzazione del clock

	/* lancio il thread di decodifica generale, apre i rispettivi codec,
	legge stream x stream e genera paccehtti audio e video che eseguiranno i distinti thread
	*/
	_demuxer = new DecodeThread(&is);				//inizializzo oggetto DecodeThread
	_demuxer->set(_demuxer);					//setto puntatore statico all'oggetto
	is.parse_tid = _demuxer;
	_demuxer->setClock(_clock);

	_demuxer->start();
	

	//nota: VideoState di default dovrebbe avere un riferimento al thread....
	if(!is.parse_tid) {
		//av_free(&is);
		qDebug() << "bla";
		exit(1);
	}

	while(1) {

		SDL_WaitEvent(&event);
		switch(event.type) {

			case FF_QUIT_EVENT:
				case SDL_QUIT:
				is.quit = 1;
				/*
				   * If the video has finished playing, then both the picture and
				   * audio queues are waiting for more data.  Make them stop
				   * waiting and terminate normally.
				*/
				SDL_CondSignal(is.audioq.GetCond());
				//SDL_CondSignal(is.videoq.cond());
				//is.audioq.GetCond()->wakeAll();
				is.videoq.GetCond()->wakeAll();

				SDL_Quit();
				exit(0);
				break;

			/*case FF_ALLOC_EVENT:
				alloc_picture(event.user.data1);
				break;*/

			/* evento del refresh della finestra */
			case FF_REFRESH_EVENT:
				video_refresh_timer(event.user.data1);
				break;

			default:
				break;
		}
	}

  return;

}

//////////////////////////////////////////////////////////////////////////////////////////////////

/**
inizializzaizone della finestra openGL
*/
int GUIThread::initializeSDL(){

	if(SDL_Init(SDL_INIT_EVERYTHING)) {							//Inizializzo SDL per visualizzazione
		qDebug() << "Impossibile inizializzare le SDL " << SDL_GetError();
		return -1;
	}

	//#ifndef __DARWIN__																			//Crea il riquadro video
	//	screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
	//#else
	//	screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
	//#endif
	//if(!screen) {																				//Se non riesco do errore
	//	std::cout << "Impossibile impostare lo schermo";
	//	exit(1);
	//

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//imposto le dimensioni della finestra
	SDL_SetVideoMode(640, 480, 32, SDL_OPENGL | (_fullscreen ? SDL_FULLSCREEN : 0));

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

std::string GUIThread::getSourceFilename(){

	return this->_fileName;
}

/**
	setto il nome del file
	particolare viene salvato sia in formato std::string
	che in const char per utilizzo in decodifcatore ffmpeg
	@param QString
*/
void GUIThread::setSourceFilename(const QString &filename){

	//passo da QString a char* per compatibilità di ffmpeg
	QByteArray bytes = filename.toUtf8();
	const char* video = bytes.data();
	_fileName = filename.toStdString();

	qDebug() << "opening video: " << filename;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////