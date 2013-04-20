#include "VideoThread.h"

//COSTRUTTORE
VideoThread::VideoThread(QObject *parent): QThread(parent)
{
	pts = 0;
	packet = &pkt1;
	_is = NULL;
}

void VideoThread::run(){

	/* alloco i frame YVU e RGB */
	pFrame = avcodec_alloc_frame();
	pFrameRGB = avcodec_alloc_frame();

	/* da questo momento in poi permetto alla finestra di resfreshare */
	_is->window->startdisplay();

	//Calculate the size in bytes that a picture of the given width and height would occupy if stored in the given picture format.
	bytes = avpicture_get_size(CONV_FORMAT, _is->video_st->codec->width, _is->video_st->codec->height);

	uint8_t *video_buffer = (uint8_t*)av_malloc( bytes * sizeof(uint8_t) );
	
	avpicture_fill((AVPicture *)pFrameRGB, video_buffer, CONV_FORMAT, _is->video_st->codec->width, _is->video_st->codec->height);

	/*
	ciclo di lettura dei frame
	prelevo dalla coda dei pkt
	decodifico il frame YUV
	trasformo il frame in RGB
	aggiungo il frameRGB alla nuova coda
	*/
	while(1) {

		// leggo i paccehtti dalla coda
		if(_is->videoq.Get(packet, 1) < 0){
			// means we quit getting packets
			qDebug() << "qutting getting packets - videothread";
			break;
		}

		//controllo se ho letto pacchetto di FLUSH
		if(packet->data == _is->flush_pkt.data){
			avcodec_flush_buffers(_is->video_st->codec);
			continue;
		}

		pts = 0;									//resetto il pts a 0, ovvero non trovato

		//Save global pts to be stored in pFrame in first call
		_is->global_video_pkt_pts = packet->pts;
		
		// Decode video frame
		avcodec_decode_video2(_is->video_st->codec, pFrame, &frameFinished, packet);

		//nota: opaque è una variabile interna a pFrame lasciata libera per essere usata dall'utente come variabile di appoggio per dei dati
		
		/* caso in cui non riesco a reperire DTS */
		if (packet->dts == (int64_t)AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE)
        {
            pts = *(uint64_t *) pFrame->opaque;
        }
		/* caso in cui riesco a reperire DTS */
        else if (packet->dts != (int64_t)AV_NOPTS_VALUE)
        {
            pts = packet->dts;
        }
        else
        {
            pts = 0;
        }
        pts *= av_q2d(_is->video_st->time_base);					//converte il tempo in double

		// Did we get a video frame?
		if(frameFinished) {

			synchronize_video();								//sincronizzazione del PTS

			/* conversione pFrame -> pFrameRGB */
			sws_scale(_is->sws_ctx, (uint8_t const * const *)pFrame->data,
				pFrame->linesize, 0, _is->video_st->codec->height, pFrameRGB->data, 
						pFrameRGB->linesize);

			/* aggiunta del frame RGB alla nuova coda */
			if(_is->pictq.Put(pFrameRGB, pts) < 0) {
				qDebug() << "quitting putting frame - videothread";
				break;
			}

		}
		av_free_packet(packet);
	}

	av_free(pFrame);
	av_free(pFrameRGB);

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////

void VideoThread::SetVideoState(VideoState *is){

	_is = is;
};

////////////////////////////////////////////////////////////////////////////////////////////

/**
funzione che aggiorna i PTS in modo tale che siano sincronizzati con tutto
devo quindi occuparsi di 2 problemi:
1) la ripetizione di uno stesso frame
2) sincronizzazione del video all'audio
per fare cio sfrutto video_clock che mantiene traccia tra quanto aspetto il prossimo frame
*/
void VideoThread::synchronize_video()
{
    double frame_delay;					//ritardo del frame

    if (pts != 0)
    {
        /* if we have pts, set video clock to it */
        _is->video_clock = pts;
    }
    else
    {
        /* if we aren't given a pts, set it to the clock */
        pts = _is->video_clock;
    }

	//faccio questi 2 passaggi per ottenere il frame_delay in double
    /* update the video clock */
    frame_delay = av_q2d(_is->video_st->codec->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    frame_delay += pFrame->repeat_pict * (frame_delay * 0.5);

	//vado cosi ad aggiornare il video_clock
    _is->video_clock += frame_delay;

}

////////////////////////////////////////////////////////////////////////////////////////////
