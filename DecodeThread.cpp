#include "DecodeThread.h"

DecodeThread *static_rif;		//Riferimento statico all'oggetto stesso

DecodeThread::DecodeThread(QObject *parent): QThread(parent)
{
	/* copia di puntatori */
	_is = NULL;

	_packet = &_pkt1;
	_pFormatCtx = NULL;					//variabile temporanea per codec
	
}

void DecodeThread::set(DecodeThread *t){
	static_rif = t;
}

int decode_interrupt_cb(void *opaque){

	return 0;
	//return (global_video_state && global_video_state->quit)
}

////////////////////////////////////////////////////////////////////////////////////////////
// ridefinizione dei metodi utilizzati da ffmpeg per reperire il frame buffer,
// in particolare, ogni volta che leggiamo un PACCHETTO viene salvato il valore PTS

/**
 * These are called whenever we allocate a frame
 * buffer. We use this to store the global_pts in
 * a frame at the time it is allocated.
 */
int our_get_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    int ret = avcodec_default_get_buffer(c, pic);					//funzione di default di ffmpeg
    uint64_t *pts = (uint64_t*)av_malloc(sizeof(uint64_t));
    *pts = static_rif->is()->global_video_pkt_pts;
    pic->opaque = pts;
    return ret;
}

void our_release_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    if(pic){
        av_freep(&pic->opaque);										//libera la memoria e setta il puntatore a NULL
	}
    avcodec_default_release_buffer(c, pic);
}

////////////////////////////////////////////////////////////////////////////////////////////

void DecodeThread::run(){
	
	video_index = -1;					//reset degli indici
	audio_index = -1;

	io_dict = nullptr;
	
	_is->videoStream = -1;				//reset degli indici
	_is->audioStream = -1;

	//qua inizializzo la variabile globale di videostate
	_is->setGlobalVideoState(_is);	

	// will interrupt blocking functions if we quit!
	callback.callback = decode_interrupt_cb;			//ncecessario CAST?
	callback.opaque = _is;

	//inizializzo io_context passandogli il file da riprodurre
	if (avio_open2(&_is->io_context, _is->getSourceFilename().c_str(), 0, &callback, &io_dict)){	
		qDebug() << "Unable to open I/O for " << QString::fromStdString(_is->getSourceFilename());
		exit(1);
	}

	/* APERTURA FILE VIDEO */
	if(avformat_open_input(&_pFormatCtx, _is->getSourceFilename().c_str(), NULL, NULL) != 0){	//Apro il file
		qDebug() << "Impossibile aprire il file";		
		exit(1);
	}

	_is->pFormatCtx = _pFormatCtx;

	if(avformat_find_stream_info(_pFormatCtx, NULL)<0){											//Leggo le informazioni sullo stream
		qDebug() << "Impossibile leggere le info sullo stream";	
		exit(1);
	}

	av_dump_format(_pFormatCtx, 0, _is->getSourceFilename().c_str(), 0);						//Mostro a video le info sul file

	//find the first video stream
	for(int i=0; i<_pFormatCtx->nb_streams; ++i){
		if(_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO && video_index<0)
			video_index=i;
	
		if(_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO && audio_index<0)
			audio_index=i;
	}
	
	if(audio_index >= 0) {
		stream_component_open(audio_index);
	}
	if(video_index >= 0) {
		stream_component_open(video_index);
	}   

	if(_is->videoStream < 0 || _is->audioStream < 0) {
		qDebug() << "could not open codecs " << QString::fromStdString(_is->getSourceFilename());
		fail();		//richiamo la funzione che mi va a chiudere la finestra
	}

	//main decode loop
	while(1){

		if(_is->quit){
			break;
		}

		//seek stuff goes here
		//qui lui inzialmente usava un peso proprio totale della lista in byte, noi usiamo solo numero di elementi
		//bisogna ridurre le 2 costanti
		if(_is->audioq.getSize() > MAX_AUDIOQ_SIZE || _is->videoq.GetSize() > MAX_VIDEOQ_SIZE) {
			//SDL_Delay(10);	//faccio una sleep
			this->usleep(10);
			continue;
		}

		if(av_read_frame(_is->pFormatCtx, _packet) < 0) {                       //leggo il frame sucessivo
			if(_is->pFormatCtx->pb->error == 0) {
				//SDL_Delay(100); /* no error; wait for user input */
				this->usleep(100);
				continue;
			} else {
				break;
			}
		}

		/* aggiunta del pacchetto alla rispettiva coda */
		if(_packet->stream_index == _is->videoStream) {								//controllo se il frame è VIDEO
			_is->videoq.Put(_packet);
		} 
		else if(_packet->stream_index == _is->audioStream) {						//se è AUDIO lo metto nella rispettiva coda
			_is->audioq.Put(_packet);
		} 
		else {
			av_free_packet(_packet);
		}
	}

	/* all done - wait for it*/
	while(!_is->quit){
		this->sleep(100);
	}

	fail();

	return;
};

//funzione eseguita in caso di errore
void DecodeThread::fail(void){

	SDL_Event event;
	event.type = FF_QUIT_EVENT;
	event.user.data1 = _is;
}

int DecodeThread::stream_component_open(int stream_index){

	AVFormatContext *pFormatCtx = _is->pFormatCtx;
	AVCodecContext *codecCtx = NULL;
	AVCodec *codec = NULL;
	AVDictionary *optionsDict = NULL;
	SDL_AudioSpec wanted_spec, spec;

	if(stream_index < 0 || stream_index >= pFormatCtx->nb_streams) {
		return -1;
	}	

	// Get a pointer to the codec context for the video stream
	codecCtx = pFormatCtx->streams[stream_index]->codec;

	/* caso codec AUDIO, vado a settare i parametri per SDL AUDIO */
	if(codecCtx->codec_type == AVMEDIA_TYPE_AUDIO) {
		// Set audio settings from codec info
		wanted_spec.freq = codecCtx->sample_rate;
		wanted_spec.format = AUDIO_S16SYS;
		wanted_spec.channels = codecCtx->channels;
		wanted_spec.silence = 0;
		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
		wanted_spec.callback = audio_callback;
		wanted_spec.userdata = _is;
    
		if(SDL_OpenAudio(&wanted_spec, &spec) < 0) {				//apertura backend audio
			qDebug() << "SDL_OpenAudio: " << SDL_GetError();
			return -1;
		}
	}

	//sia nel caso VIDEO che AUDIO vado a caricare rispettivo codec
	codec = avcodec_find_decoder(codecCtx->codec_id);
	if(!codec || (avcodec_open2(codecCtx, codec, &optionsDict) < 0)) {
		fprintf(stderr, "Unsupported codec!\n");
		return -1;
	}

	switch(codecCtx->codec_type) {

		//AUDIO
		case AVMEDIA_TYPE_AUDIO:
			_is->audioStream = stream_index;
			_is->audio_st = pFormatCtx->streams[stream_index];
			_is->audio_buf_size = 0;
			_is->audio_buf_index = 0;
			memset(&_is->audio_pkt, 0, sizeof(_is->audio_pkt));
			_is->audioq = PacketQueueAudio();											//inizializzo la coda di paccheti AUDIO
			SDL_PauseAudio(0);
			break;

		//VIDEO
		case AVMEDIA_TYPE_VIDEO:
			_is->videoStream = stream_index;
			_is->video_st = pFormatCtx->streams[stream_index];

			//imposto le dimensioni della pagina
			_is->window->setSize(_is->video_st->codec->width, _is->video_st->codec->height);

			//inizializzazione del timer del computer
			_is->frame_timer = (double) av_gettime()/1000000.0;
			_is->frame_last_delay = 40e-3;
    
			_is->videoq = PacketQueueVideo();											//inizializzo la coda di frame VIDEO
			_video_th = new VideoThread();											//inizializzo il thread di riproduzione video
			_video_th->SetVideoState(_is);
			_video_th->start();														//mando in esecuzione il thread decodifica video

			//genera il context se non c'è gia, e se gia presente fa un confronto
			_is->sws_ctx = sws_getCachedContext(_is->sws_ctx, 
				_is->video_st->codec->width, 
				_is->video_st->codec->height, 
				_is->video_st->codec->pix_fmt, 
				_is->video_st->codec->width, 
				_is->video_st->codec->height, 
				CONV_FORMAT, 
				SWS_BICUBIC, 
				NULL, 
				NULL, 
				NULL);

			//dal secondo frame in poi utilizziamo la nostra riscritta funzione per allocare e deallocare il codec buffer
			//richiama il metodo di default nel caso ci venga passato il DTS, se no usiamo il PTS del primo pacchetto
			codecCtx->get_buffer = our_get_buffer;
			codecCtx->release_buffer =  our_release_buffer;

			break;

		default:
			break;
	}
	return 0;
};

////////////////////////////////////////////////////////////////////////////////////////////

VideoState* DecodeThread::is(){
	return _is;
}

////////////////////////////////////////////////////////////////////////////////////////////

void DecodeThread::SetVideoState(VideoState *is){

	_is = is;
};

