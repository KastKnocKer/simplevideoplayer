#include "DecodeThread.h"

DecodeThread *static_rif;		//Riferimento statico all'oggetto stesso
VideoState *global_video_state;

//COSTRUTTORE
DecodeThread::DecodeThread(QObject *parent): QThread(parent)
{
	/* copia di puntatori */
	_is = NULL;

	_packet = &_pkt1;
	_pFormatCtx = NULL;					//codec
	global_video_state = NULL;
}

/*
metodo per settare il riferimento statico
*/
void DecodeThread::set(DecodeThread *t){
	static_rif = t;
}

int decode_interrupt_cb(void *opaque){
	return (global_video_state && global_video_state->ut.getStopValue());
}

////////////////////////////////////////////////////////////////////////////////////////////
// ridefinizione dei metodi utilizzati da ffmpeg per reperire il frame buffer,
// in particolare, ogni volta che leggiamo un PACCHETTO viene salvato il valore PTS

/**
 * Metodo richiamato in automatico da @see avcodec_decode_video()
 * ogni volta che viene allocato un frame buffer, ovvero al prima pacchetto del buffer.
 * Lo facciamo per ottenere il PTS del pacchetto e inserirlo nel frame.
 */
int our_get_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    int ret = avcodec_default_get_buffer(c, pic);					//funzione di default di ffmpeg
    uint64_t *pts = (uint64_t*)av_malloc(sizeof(uint64_t));
	*pts = static_rif->GetVideoState()->global_video_pkt_pts;
    pic->opaque = pts;
    return ret;
}

/**
 * Metodo di rilascio del buffer contenente il frame
*/
void our_release_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    if(pic){
        av_freep(&pic->opaque);										//libera la memoria e setta il puntatore a NULL
	}
    avcodec_default_release_buffer(c, pic);
}

////////////////////////////////////////////////////////////////////////////////////////////

/*
codice che viene eseguito dal thread
*/
void DecodeThread::run(){
	
	video_index = -1;					//reset degli indici
	audio_index = -1;

	io_dict = NULL;
	
	_is->videoStream = -1;				//reset degli indici
	_is->audioStream = -1;

	//qua inizializzo la variabile globale di videostate
	global_video_state = _is;

	// will interrupt blocking functions if we quit!
	_pFormatCtx = avformat_alloc_context();
	_pFormatCtx->interrupt_callback.callback = decode_interrupt_cb;
	_pFormatCtx->interrupt_callback.opaque = _is;

	//inizializzo io_context passandogli il file da riprodurre
	if (avio_open2(&_is->io_context, _is->getSourceFilename().c_str(), AVIO_FLAG_READ, &_pFormatCtx->interrupt_callback, &io_dict)){	
		//qDebug() << "Unable to open I/O for " << QString::fromStdString(_is->getSourceFilename());
		fail();
		return;
	}

	/* APERTURA FILE VIDEO */
	if(avformat_open_input(&_pFormatCtx, _is->getSourceFilename().c_str(), NULL, NULL) != 0){	//Apro il file
		//qDebug() << "Impossibile aprire il file";		
		fail();
		return;
	}

	_is->pFormatCtx = _pFormatCtx;

	

	if(avformat_find_stream_info(_pFormatCtx, NULL)<0){											//Leggo le informazioni sullo stream
		//qDebug() << "Impossibile leggere le info sullo stream";	
		fail();
		return;
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
		
		//qDebug() << "could not open codecs " << QString::fromStdString(_is->getSourceFilename());
		fail();		//richiamo la funzione che mi va a chiudere la finestra
		return;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// INIZIO CICLO DECODIFICA

	//main decode loop
	while(1){

		//controllo per STOP
		if(_is->ut.getStopValue() == true){
			
			//qDebug() << "decodethread - stopvalue";
			
			break;
		}

		//controllo per PAUSE
		if(_is->ut.isPauseChanged()){
			_is->ut.setLastPauseValue(_is->ut.getPauseValue());
			if(_is->ut.getPauseValue() == true){
				
				//qDebug() << "DecodeThread - av_read_pause";				
				_is->read_pause_return = av_read_pause(_pFormatCtx);
			} else {
				
				//qDebug() << "DecodeThread - av_read_play";
				av_read_play(_pFormatCtx);
			}
		};

		//SEEK - controllo se ho una richiesta di seek
		if(_is->seek_req){

			int stream_index = -1;	//reset variabile identifica lo stream

			int64_t seek_target = _is->seek_pos; //nuovo tempo desiderato
			int64_t DesiredFrameNumber;

			if(_is->videoStream >= 0){
				stream_index = _is->videoStream;
			} 
			else if(_is->audioStream >= 0){
				stream_index = _is->audioStream;
			}

			if(stream_index >= 0){
					
				//convert time into frame number
				DesiredFrameNumber = av_rescale(seek_target, _pFormatCtx->streams[stream_index]->time_base.den, 
					_pFormatCtx->streams[stream_index]->time_base.num);
				DesiredFrameNumber /= AV_TIME_BASE;
				
			}
      
			//effettuo il seek
			if(avformat_seek_file(_is->pFormatCtx, stream_index, 0, DesiredFrameNumber, DesiredFrameNumber, AVSEEK_FLAG_FRAME) < 0) {		
				//qDebug() << "SEEKING ERROR" << (long long) DesiredFrameNumber;	
			} 
			else {
				
				//qDebug() << "SEEKING SUCCESS" << (long long) DesiredFrameNumber;

				//svuoto le liste e inserisco pacchetto di flush
				if(_is->audioStream >= 0) {
					_is->audioq.Flush();
					_is->audioq.Put(_is->flush_pkt);
				}
				if(_is->videoStream >= 0) {
					_is->videoq.Flush();
					_is->videoq.Put(_is->flush_pkt);
				}
			}

			_is->seek_req = 0;			//resetto a 0 (false) la richiesta di seek
			_is->ut.setEOFValue(false);
		}

		if(_is->audioq.getSize() > MAX_AUDIOQ_SIZE || _is->videoq.getSize() > MAX_VIDEOQ_SIZE) {
			this->usleep(10000);
			continue;
		}

		// EOF
		if(_is->ut.getEOFValue() == true){
			
			//qDebug() << "END OF FILE - DECODING";

			if(_is->videoStream >= 0){
				av_init_packet(_packet);
                _packet->data = NULL;
                _packet->size = 0;
                _packet->stream_index = _is->videoStream;
				_is->videoq.Put(_packet);
            }
			this->usleep(10000);
            if(_is->audioq.getSize() + _is->videoq.getSize() == 0){
				
				//qDebug() << "EOF - liste vuote";
				
				fail();
				return;
			}
		}


		if(av_read_frame(_is->pFormatCtx, _packet) < 0) {                       //leggo il frame sucessivo

			//� stata raggiunta la fine del file
			if(_is->pFormatCtx->pb->error == 0) {

				_is->ut.setEOFValue(true);										//setto il flag di fine file
				continue;

			//errore di lettura del pacchetto
			} else {
				//qDebug() << "ERROR - av_read_frame";
				break;
			}
		}

		/* aggiunta del pacchetto alla rispettiva coda */
		if(_packet->stream_index == _is->videoStream) {								//controllo se il frame � VIDEO
			_is->videoq.Put(_packet);
		} 
		else if(_packet->stream_index == _is->audioStream) {						//se � AUDIO lo metto nella rispettiva coda
			_is->audioq.Put(_packet);
		} 
		else {
			av_free_packet(_packet);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//FINE CICLO DECODIFICA

	//qDebug() << "DecodeThread - esco dal ciclo di decodifica";
	
	/* all done - wait for it */
	while(_is->ut.getStopValue() != true){
		this->usleep(100000);
	}

	fail();

	return;
};

//funzione eseguita in caso di errore/chiusura
void DecodeThread::fail(void){
	
	//qDebug() << "fail!!";
	
	emit eof();
}

////////////////////////////////////////////////////////////////////////////////////////////

/* questo metodo e usato sia per lo stream audio che video, ed esegue:
	- apertura codec
	- setting codec a VideoState
	- [nel caso audio apertura di SDL_OpenAudio]
	- inizializzazione del rispettivo thread di riproduzione audio/video
*/
int DecodeThread::stream_component_open(int stream_index){

	AVFormatContext *pFormatCtx = _is->pFormatCtx;
	AVCodecContext *codecCtx = NULL;
	AVCodec *codec = NULL;
	AVDictionary *optionsDict = NULL;
	SDL_AudioSpec wanted_spec, spec;

	if(stream_index < 0 || stream_index >= pFormatCtx->nb_streams) {
		return -1;
	}	

	// Get a pointer to the codec context for the video/audio stream
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
		wanted_spec.userdata = _clock;
    
		if(SDL_OpenAudio(&wanted_spec, &spec) < 0) {				//apertura backend audio
			
			//qDebug() << "SDL_OpenAudio: " << SDL_GetError();
			
			return -1;
		}

		_is->audio_hw_buf_size = spec.size;
	}

	//sia nel caso VIDEO che AUDIO vado a caricare rispettivo codec
	codec = avcodec_find_decoder(codecCtx->codec_id);
	if(!codec || (avcodec_open2(codecCtx, codec, &optionsDict) < 0)) {
		
		//qDebug() << "Unsupported codec!";
		
		return -1;
	}

	switch(codecCtx->codec_type) {

		//AUDIO
		case AVMEDIA_TYPE_AUDIO:
			_is->audioStream = stream_index;
			_is->audio_st = pFormatCtx->streams[stream_index];
			_is->audio_buf_size = 0;
			_is->audio_buf_index = 0;

			//qDebug() << "sample rate: " << codecCtx->sample_rate;
			//qDebug() << "channels: " << codecCtx->channels;

			 /* averaging filter for audio sync */
			_is->audio_diff_avg_coef = exp(log(0.01 / AUDIO_DIFF_AVG_NB));
			_is->audio_diff_avg_count = 0;
			/* Correct audio only if larger error than this */
			_is->audio_diff_threshold = 2.0 * SDL_AUDIO_BUFFER_SIZE / codecCtx->sample_rate;

			memset(&_is->audio_pkt, 0, sizeof(_is->audio_pkt));
			
			SDL_PauseAudio(0);
			break;

		//VIDEO
		case AVMEDIA_TYPE_VIDEO:
			_is->videoStream = stream_index;
			_is->video_st = pFormatCtx->streams[stream_index];

			//ottengo il numero totale di frame dello stream video
			_is->totalFramesNumber = _is->video_st->nb_frames;
			
			//qDebug() << "num TOT frame video" << (long long) _is->video_st->nb_frames;

			//reperisco informazioni sulla durata e le vado a impostare allo slider
			//CASO FAVOREVOLE: trovo direttamente il valore della durata del video
			duration = (int) (_pFormatCtx->duration/AV_TIME_BASE);
			//CASO SFAVOREVOLE: calcolo la durata come num_frame * frame_rate
			if(duration <= 0){
				duration = (int) av_rescale(_is->totalFramesNumber, _pFormatCtx->streams[stream_index]->time_base.num, 
					_pFormatCtx->streams[stream_index]->time_base.den);
				duration = duration/AV_TIME_BASE;
			}
			emit setSliderRange(0, duration);		//durata max del video

			/*qDebug() << "durata del video: " << duration;
			qDebug() << "time_base: " << (double) av_q2d(_is->video_st->time_base);
			qDebug() << "avg_frame_rate: " << av_q2d(_is->video_st->avg_frame_rate);*/
			
			//imposto le dimensioni della pagina
			_is->window->setSize(_is->video_st->codec->width, _is->video_st->codec->height);

			//inizializzazione del timer del computer
			_is->frame_timer = (double) av_gettime()/1000000.0;
			_is->frame_last_delay = 40e-3;
			_is->video_current_pts_time = av_gettime();								//inizializzazioen del timer per video clcok
    
			_video_th = new VideoThread();											//inizializzo il thread di riproduzione video
			_video_th->SetVideoState(_is);
			_video_th->start();														//mando in esecuzione il thread decodifica video

			//genera il context se non c'� gia, e se gia presente fa un confronto
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

VideoState* DecodeThread::GetVideoState(){
	return _is;
}

void DecodeThread::SetVideoState(VideoState *is){

	_is = is;
};

////////////////////////////////////////////////////////////////////////////////////////////

AVClock* DecodeThread::GetAVClock(){
	return _clock;
}

void DecodeThread::SetAVClock(AVClock *c){

	_clock = c;
};

