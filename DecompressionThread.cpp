/**
* @file DecompressionThread.cpp
* @brief classe contenente le funzioni eseguite dal thread di decompressione.
* @author Matteo Renzi
* @data 13/11/2012
*/

#include "DecompressionThread.h"

DecompressionThread *static_rif;		//Riferimento statico all'oggetto stesso

//COSTRUTTORE
DecompressionThread::DecompressionThread(){

	//imposto che di default il fullscreen non parta
	fullscreen = 0;

	//setto i puntatori a NULL
	pSWSContext = NULL;
	_pFormatCtx = NULL;
	pCodec = NULL;
	pCodecCtx = NULL;
	audioOptDict = NULL;
	videoOptDict = NULL;
	pFrame = 0;
	pFrameRGB = 0;
	bmp = NULL;
	screen = NULL;
};

//DISTRUTTORE
DecompressionThread::~DecompressionThread(){

};

/**
	metodo per settare la variabile globale decompressionThread
*/
void DecompressionThread::set(DecompressionThread *t){
	static_rif = t;
}

/*
	metood per lanciare il thread
*/
void DecompressionThread::start(){

	//inizializzo il thread
	_thread = std::thread(&DecompressionThread::run, this);
	//_thread.join();

	return;
}

/*
	funzione utilizzata dal thread di decompressione per caricare il video e decomprimerlo
*/
void DecompressionThread::run(){

	SDL_Event event;

	//manager = Utility();									//genero la classe utility

	//inizializzione del decodificatore
	initializeFFMPEG();

	//imposto le dimensioni come quelle del video o a fullscreen
	width = fullscreen ? 1920 : pCodecCtx->width;
	height = fullscreen ? 1200 : pCodecCtx->height;

	initializeSDL();
	
	initializeGL();																				
	
	//genera il context se non c'è gia, e se gia presente fa un confronto
	pSWSContext = sws_getCachedContext(pSWSContext, pCodecCtx->width, pCodecCtx->height, 
      pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, CONV_FORMAT, 
      SWS_BICUBIC, NULL, NULL, NULL);

	while(1){

		/* gestore eventi chiusura finestra SDL */
		while( SDL_PollEvent(&event) )
		{
			switch(event.type) {
			  case SDL_QUIT:
				SDL_Quit();
				exit(0);
				break;
			  case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
				  SDL_Quit();
				  exit(0);
				}
				break;
			 /*serve per il resize della finestra*/
			  case SDL_VIDEORESIZE:
				 //funziona anche se non ci metto l'if
				 if(SDL_SetVideoMode(event.resize.w, event.resize.h, 32, SDL_OPENGL | SDL_RESIZABLE )){
					width=event.resize.w;
					height=event.resize.h;
					initializeGL();
					// mi viene il dubbio che qui dentro ci entri sempre... controlla
					}
					break;
			  default:
				break;
			}
		}

		//ottengo un frame
		next_frame();																			

		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture( GL_TEXTURE_2D, _texture_video );
		glScalef(1.0f, -1.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f); 
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
		glScalef(1.0f, -1.0f, 1.0f);

		SDL_GL_SwapBuffers();

	}

	return;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
/* AUDIO */
//////////////////////////////////////////////////////////////////////////////////////////////////

int DecompressionThread::audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {

  static AVPacket pkt;
  static uint8_t *audio_pkt_data = NULL;
  static int audio_pkt_size = 0;
  //codice aggiunto
  static AVFrame frame;

  int len1, data_size = 0;

  for(;;) {
		while(audio_pkt_size > 0) {
			int got_frame = 0;
			data_size = buf_size;
			len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);		//Decode the audio frame of size avpkt->size from avpkt->data into frame
			if(len1 < 0) {
				/* if error, skip frame */
				audio_pkt_size = 0;
				break;
			}
			audio_pkt_data += len1;
			audio_pkt_size -= len1;
			if(got_frame){
				data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame.nb_samples, aCodecCtx->sample_fmt, 1);
				memcpy(audio_buf, frame.data[0], data_size);
			}
			if(data_size <= 0) {
				/* No data yet, get more frames */
				continue;
			}
			/* We have data, return it and come back for more later */
			return data_size;
		}

		if(pkt.data)
		  av_free_packet(&pkt);

		//if(manager.getStopValue()) { //if(quit)
		//  return -1;
		//}

		//if(packet_queue_get(&audioq, &pkt, 1) < 0){
		if(audioq.Get(&pkt, 1) < 0) {
		  return -1;
		}

		audio_pkt_data = pkt.data;
		audio_pkt_size = pkt.size;
	}
}


/**
metodo interno utilizzato dalla finestra SDL per aggiornare lo stato dello stream audio
*/
void audio_callback(void *userdata, Uint8 *stream, int len) {

	AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
	int len1, audio_size;

	static uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	static unsigned int audio_buf_size = 0;
	static unsigned int audio_buf_index = 0;

	while(len > 0) {
		if(audio_buf_index >= audio_buf_size) {
			/* We have already sent all our data; get more */
			audio_size = static_rif->audio_decode_frame(aCodecCtx, audio_buf, audio_buf_size);
			if(audio_size < 0) {
				/* If error, output silence */
				audio_buf_size = 1024; // arbitrary?
				memset(audio_buf, 0, audio_buf_size);
			} else {
				audio_buf_size = audio_size;
			}
			audio_buf_index = 0;
		}
		len1 = audio_buf_size - audio_buf_index;
		if(len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
		len -= len1;
		stream += len1;
		audio_buf_index += len1;
	}
}

/**
metodo esterno che funge da wrapper per utilizzo del metodo callback interno a una classe
*/
/*void audio_callback_wrapper(void *decompression, Uint8 *stream, int len){
	((class DecompressionThread *)decompression)->audio_callback(decompression, stream, len);
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////
//INIZIALIZZAZONE
//////////////////////////////////////////////////////////////////////////////////////////////////

void DecompressionThread::initializeFFMPEG(){

	av_register_all();																			//Registro i codec

	if(avformat_open_input(&_pFormatCtx, this->getSourceFilename().c_str(), NULL, NULL) != 0){	//Apro il file
		qDebug() << "Impossibile aprire il file";		
		exit(1);
	}

	if(avformat_find_stream_info(_pFormatCtx, NULL)<0){											//Leggo le informazioni sullo stream
		qDebug() << "Impossibile leggere le info sullo stream";	
		exit(1);
	}

	av_dump_format(_pFormatCtx, 0, this->getSourceFilename().c_str(), 0);						//Mostro a video le info sul file

	/* find the first video stream */
	videoStream=-1;																				//Cerco il primo stream video
	audioStream=-1;
	//for(uint8_t i=0; i<_pFormatCtx->nb_streams; ++i){
	for(int i=0; i<_pFormatCtx->nb_streams; ++i){
		if(_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO && videoStream<0)
			videoStream=i;
	
		if(_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO && audioStream<0)
			audioStream=i;
	}
	if(videoStream==-1){																		//Non ha trovato niente
		qDebug() << "Video stream non trovato";		
		exit(1);
	}
	if(audioStream==-1){
		qDebug() << "Audio stream non trovato";
		exit(1);
	}

	aCodecCtx = _pFormatCtx->streams[audioStream]->codec;										//ottengo il CODEC AUDIO

	// Set audio settings from codec info
	wanted_spec.freq = aCodecCtx->sample_rate;													//Ottengo info sul sample rate
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = aCodecCtx->channels;													//Numero di canali
	wanted_spec.silence = 0;
	wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
	wanted_spec.callback = audio_callback;														//vado a indicare la funzione di callback
	wanted_spec.userdata = aCodecCtx;															//setto il context audio
  
	if(SDL_OpenAudio(&wanted_spec, &spec) < 0) {
		qDebug() << "SDL_OpenAudio: " << SDL_GetError();
		exit(1);
	}

	aCodec = avcodec_find_decoder(aCodecCtx->codec_id);											//cerco il codec
	if(!aCodec) {
		qDebug() << "Unsupported codec!";
		exit(1);
	}

	avcodec_open2(aCodecCtx, aCodec, &audioOptDict);											//apro il codec AUDIO

	/* creo una coda per i pacchetti audio */
	//packet_queue_init(&audioq);
	audioq = PacketQueue2();
	SDL_PauseAudio(0);

	pCodecCtx = _pFormatCtx->streams[videoStream]->codec;										//Leggo il puntatore dal codec per lo stream video

	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);											//Cerco un decoder corrispettivo per lo stream video
	if(pCodec==NULL) {																			//Se non lo trovo, do errore
		qDebug() << "Codec non supportato";		
		exit(1);
	}
	
	if(avcodec_open2(pCodecCtx, pCodec, &videoOptDict) < 0){										//Apro il codec VIDEO
		qDebug() << "Impossibile aprire il codec";												//Se non lo riesco ad aprire do errore		
		exit(1);
	}
	pFrame = avcodec_alloc_frame();																//Alloco il frame video
	pFrameRGB = avcodec_alloc_frame();															//Alloco il frame video convertito in RGB

	//Calculate the size in bytes that a picture of the given width and height would occupy if stored in the given picture format.
	bytes = avpicture_get_size(CONV_FORMAT, pCodecCtx->width, pCodecCtx->height);

	uint8_t *video_buffer = (uint8_t*)av_malloc( bytes * sizeof(uint8_t) );
	
	avpicture_fill((AVPicture *)pFrameRGB, video_buffer, CONV_FORMAT, pCodecCtx->width, pCodecCtx->height);

}

//////////////////////////////////////////////////////////////////////////////////////////////////

/**
inizializzaizone della finestra openGL
*/
void DecompressionThread::initializeSDL(){

	if(SDL_Init(SDL_INIT_EVERYTHING)) {							//Inizializzo SDL per visualizzazione
		qDebug() << "Impossibile inizializzare le SDL " << SDL_GetError();
		exit(1);
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
	SDL_SetVideoMode(width, height, 32, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));

}

//////////////////////////////////////////////////////////////////////////////////////////////////

/**
inzializziamo le matrici e la texture per OpengGL
*/
void DecompressionThread::initializeGL(){

	// Start Of User Initialization
	glEnable(GL_TEXTURE_2D);
	glClearColor(1.0f, 0.0f, 0.0f, 0.2f);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);								//specifico la matrice su cui si baserà OpenGL
	glLoadIdentity();											//glLoadIdentity() replaces the current matrix with the identity matrix

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	
	glShadeModel( GL_SMOOTH );

	glGenTextures(1, &_texture_video);
	glBindTexture(GL_TEXTURE_2D, _texture_video); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pCodecCtx->width, pCodecCtx->height, 0, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}




//////////////////////////////////////////////////////////////////////////////////////////////////

/**
metodo per realizzare il ciclo dei frame sulla finestra OpenGL
*/
void DecompressionThread::next_frame(){

	while(true){																					//Leggo i frame dal video (TUTTI)

		if(av_read_frame(_pFormatCtx, &packet)>=0) {												//a ogni frame viene associato un pacchetto

			/* VIDEO */
			if(packet.stream_index==videoStream) {													//Se è un pacchetto che fa parte del video
				
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);					//Decodifico il frame

				if(frameFinished) {																	//Se ho un frame completo

					sws_scale(pSWSContext, (uint8_t const * const *)pFrame->data,					//http://ffmpeg.org/doxygen/trunk/group__lsws.html#gae531c9754c9205d90ad6800015046d74
						pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, 
						pFrameRGB->linesize);

					/*
					glBindTexture lets you create or use a named texture. Calling glBindTexture with target set to GL_TEXTURE_2D or 
					GL_TEXTURE_CUBE_MAP and texture set to the name of the new texture binds the texture name to the target of the current active texture unit.
					When a texture is bound to a target, the previous binding for that target is automatically broken.
					Texture names are unsigned integers. The value zero is reserved to represent the default texture for each texture target.
					*/
					glBindTexture( GL_TEXTURE_2D, _texture_video );									//associo la texture corrente						

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pCodecCtx->width, pCodecCtx->height, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);

					av_free_packet(&packet);			//?????
					break;
				}

			/* AUDIO */
			} else if (packet.stream_index == audioStream){
				
				//packet_queue_put(&audioq, &packet);			//aggiungo il pacchetto letto alla coda
				audioq.Put(&packet);
			/* ALTRO */
			} else 
				av_free_packet(&packet);		//Libero il pacchetto che è stato allocato per leggere il frame
		}
		else {
            av_seek_frame(_pFormatCtx, videoStream, 0,  AVSEEK_FLAG_FRAME);
        } 	


	}

};

//////////////////////////////////////////////////////////////////////////////////////////////////

std::string DecompressionThread::getSourceFilename(){

	return this->_fileName;
}

/**
	setto il nome del file
	particolare viene salvato sia in formato std::string
	che in const char per utilizzo in decodifcatore ffmpeg
	@param QString
*/
void DecompressionThread::setSourceFilename(const QString &filename){

	//passo da QString a char* per compatibilità di ffmpeg
	QByteArray bytes = filename.toUtf8();
	const char* video = bytes.data();
	_fileName = filename.toStdString();

	qDebug() << filename;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//void DecompressionThread::SetManager(Utility manager){
//
//	DecompressionThread::manager = manager;
//}

//ritorno il puntatore del manager
//Utility* DecompressionThread::GetManager(){
//
//	return &manager;
//}




