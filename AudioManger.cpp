#include "AudioManager.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
/* AUDIO */
//////////////////////////////////////////////////////////////////////////////////////////////////

/* decode one audio frame and returns its uncompressed size */
int audio_decode_frame(VideoState *is, double *pts_ptr) {

	AVPacket *pkt = &is->audio_pkt;							//Pacchetto audio in cui copiare quello della coda
	AVCodecContext *dec= is->audio_st->codec;
	int n = 0;
	int len1 = -1; 
	int data_size = 0;
	double pts;

	while(1) {

		/* questo perchè in un pacchetto audio posso essere presenti + frame audio */
		while(is->audio_pkt_size > 0) {
			int got_frame = 0;

			//Decode the audio frame of size avpkt->size from avpkt->data into frame
			len1 = avcodec_decode_audio4(dec, &is->audio_frame, &got_frame, pkt);

			if(len1 < 0){
				/* if error, skip frame */
				is->audio_pkt_size = 0;
				break;
			}
			
			if(got_frame)
			{
				data_size = av_samples_get_buffer_size(NULL, dec->channels, is->audio_frame.nb_samples, dec->sample_fmt, 1);
				memcpy(is->audio_buf, is->audio_frame.data[0], data_size);
			}

			is->audio_pkt_data += len1;
			is->audio_pkt_size -= len1;

			if(data_size <= 0)
			{
				/* No data yet, get more frames */
				continue;
			}

			/* if no pts, than compute it */
			pts = is->audio_clock;
			*pts_ptr = pts;

			n = 2 * dec->channels;
			is->audio_clock += (double) data_size / (double) (n*dec->sample_rate);

			/* We have data, return it and come back for more later */
			return data_size;
		}

		/* free the current packet */
		if(pkt->data)
			av_free_packet(pkt);

		if((is->ut.getStopValue() == true) || (is->ut.getPauseValue() == true)){
			return -1;
		}

		/* read next packet */
		if(is->audioq.Get(pkt, 1) < 0){
			return -1;
		}

		/* if we read a flush pkt */
		if(pkt->data == is->flush_pkt->data){
			
				//qDebug() << "AudioManager - letto FLUSH PKT";
			
			avcodec_flush_buffers(dec);
			continue;
		}

		is->audio_pkt_data = pkt->data;
		is->audio_pkt_size = pkt->size;

		/* if update, update the audio clock with the pts */
		if(pkt->pts != AV_NOPTS_VALUE){
			is->audio_clock = av_q2d(is->audio_st->time_base)*pkt->pts;
		}
	}
}


/**
metodo interno utilizzato da SDL per aggiornare lo stato dello stream audio
prepare a new audio buffer
*/
void audio_callback(void *userdata, Uint8 *stream, int len) {

	AVClock *clock = (AVClock *) userdata;
	VideoState *is = clock->GetVideoState();
	int len1, audio_size;
	double pts;

	while(len > 0) {
		if(is->audio_buf_index >= is->audio_buf_size) {
			/* We have already sent all our data; get more */
			audio_size = audio_decode_frame(is, &pts);
			if(audio_size < 0) {
				/* If error, output silence */
				is->audio_buf_size = 1024; // arbitrary?
				memset(is->audio_buf, 0, is->audio_buf_size);
			} else {
				audio_size = synchronize_audio(clock, is, (int16_t *) is->audio_buf, audio_size, pts);
				is->audio_buf_size = audio_size;
			}
			is->audio_buf_index = 0;
		}
		len1 = is->audio_buf_size - is->audio_buf_index;
		if(len1 > len){
			len1 = len;
		}
		//copio in audio bug una parte di audio buf index di grandezza len1
		memcpy(stream, (uint8_t *)is->audio_buf  + is->audio_buf_index, len1);
		len -= len1;
		stream += len1;
		is->audio_buf_index += len1;
	}

}

/**
funzione utilizzata in caso di VideoClock
obbiettivo e controllare se audio e video sono sincronizzati,
e nel caso non lo fossero, togleire o aggiungere sample audio

tale sincronizzazione non verra effettuata ogni singola volta che ho una variazione
la chiamero consecutivamente un numero minimo di volte

Add or subtract samples to get a better sync, return new
   audio buffer size
*/
int synchronize_audio(AVClock *clock, VideoState *is, short *samples, int samples_size, double pts) {

  int n = 0;
  double ref_clock = 0.0;

  n = 2 * is->audio_st->codec->channels;
  
  /* la sincronizzazione non deve essere fatta in caso di AudioClock */
  if(clock->clockType() != clock->AudioClock) {

		double diff, avg_diff;
		int wanted_size, min_size, max_size , nb_samples;
		
		/* calcolo della diff = audio_clock - video_clock */
		ref_clock = clock->get_master_clock();
		diff = clock->get_audio_clock() - ref_clock;

		/*
		la differenza tra audio e video non deve essere esatta
		abbiamo un valore entro il quale è considerata accettabile
		*/
		if(diff < AV_NOSYNC_THRESHOLD) {
		
			// accumulate the diffs
			is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
			if(is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
				/* not enough mesasures to have a correct estimate */
				is->audio_diff_avg_count++;

			} else {

				avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);
			
				/* controllo se la differenza supera la soglia */
				if(fabs(avg_diff) >= is->audio_diff_threshold) {

					/* calcoliamo quanti samples dobbiamo aggiungere o togliere */
					/* stiamo stimando la diffrenza A-V*/
					wanted_size = samples_size + ((int)(diff * is->audio_st->codec->sample_rate) * n);
					nb_samples = samples_size/n;	//dimensione per ogni canale

					min_size = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;
					max_size = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;
					if(wanted_size < min_size) {
						wanted_size = min_size;
					} else if (wanted_size > max_size) {
						wanted_size = max_size;
					}

					//CASO AUDIO AVANTI RISPETTO VIDEO
					if(wanted_size < samples_size) {
						/* remove samples */
						samples_size = wanted_size;
					//CASO AUDIO INDIETRO RISPETTO VIDEO
					} else if(wanted_size > samples_size) {

						uint8_t *samples_end, *q;
						int nb;

						/* add samples by copying final sample*/
						nb = (samples_size - wanted_size);
						samples_end = (uint8_t *)samples + samples_size - n;
						q = samples_end + n;
						while(nb > 0) {
							memcpy(q, samples_end, n);
							q += n;
							nb -= n;
						}
						samples_size = wanted_size;
					}
				}
			}
		} 
		else {
			/* difference is TOO big; reset diff stuff */
			is->audio_diff_avg_count = 0;
			is->audio_diff_cum = 0;
		}
  }

  /* ritorno al nuova dimensione che deve avere il sample */
  return samples_size;
}