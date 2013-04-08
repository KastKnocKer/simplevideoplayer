#include "AudioManager.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
/* AUDIO */
//////////////////////////////////////////////////////////////////////////////////////////////////

int audio_decode_frame(VideoState *is, double *pts_ptr) {

	AVPacket pkt;							//Pacchetto audio da mandare in output
	int n, len1, data_size = 0;
	double pts;

	while(is->audioq.Get(&pkt, 1) < 0 && !is->quit){;		
		//Prelevo il primo pacchetto dalla coda,
		//attende fino a che non c'è un pacchetto, è inutile proseguire...
	}
	is->audio_pkt_data = pkt.data;
	is->audio_pkt_size = pkt.size;

	while(1) {
		while(&is->audio_pkt_size > 0) {
			int got_frame = 0;
			len1 = avcodec_decode_audio4(is->audio_st->codec, &is->audio_frame, &got_frame, &pkt);		//Decode the audio frame of size avpkt->size from avpkt->data into frame
			if(len1 < 0)
			{
				/* if error, skip frame */
				is->audio_pkt_size = 0;
				break;
			}
			
			if(got_frame)
			{
				data_size = av_samples_get_buffer_size(NULL, is->audio_st->codec->channels, is->audio_frame.nb_samples, is->audio_st->codec->sample_fmt, 1);
				memcpy(is->audio_buf, is->audio_frame.data[0], data_size);
			}

			is->audio_pkt_data += len1;
			is->audio_pkt_size -= len1;

			if(data_size <= 0)
			{
				/* No data yet, get more frames */
				continue;
			}

			pts = is->audio_clock;
			*pts_ptr = pts;

			n = 2 * is->audio_st->codec->channels;
			is->audio_clock += (double) data_size / (double) (n*is->audio_st->codec->sample_rate);

			/* We have data, return it and come back for more later */
			return data_size;
		}

		if((&pkt)->data)
			av_free_packet(&pkt);

		if(is->quit){
			return -1;
		}

		/* next packet */
		if(is->audioq.Get(&pkt, 1) < 0){
		  return -1;
		}

		is->audio_pkt_data = pkt.data;
		is->audio_pkt_size = pkt.size;

		/* if update, update the audio clock w/pts */
		if(pkt.pts != AV_NOPTS_VALUE){
			is->audio_clock = av_q2d(is->audio_st->time_base)*pkt.pts;
		}
	}
}


/**
metodo interno utilizzato da SDL per aggiornare lo stato dello stream audio
*/
void audio_callback(void *userdata, Uint8 *stream, int len) {

	AVClock2 *clock = (AVClock2 *) userdata;
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
		memcpy(stream, (uint8_t *)is->audio_buf  + is->audio_buf_index, len1);
		len -= len1;
		stream += len1;
		is->audio_buf_index += len1;
	}
}

/* Add or subtract samples to get a better sync, return new
   audio buffer size */
int synchronize_audio(AVClock2 *clock, VideoState *is, short *samples, int samples_size, double pts) {

  int n;
  double ref_clock;

  n = 2 * is->audio_st->codec->channels;
  
  if(clock->clockType() != clock->AudioClock) {
		double diff, avg_diff;
		int wanted_size, min_size, max_size /*, nb_samples */;
    
		ref_clock = clock->get_master_clock();
		diff = clock->get_audio_clock() - ref_clock;

    if(diff < AV_NOSYNC_THRESHOLD) {
		// accumulate the diffs
		is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
		if(is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
			is->audio_diff_avg_count++;
		} else {
			avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);
			if(fabs(avg_diff) >= is->audio_diff_threshold) {
				wanted_size = samples_size + ((int)(diff * is->audio_st->codec->sample_rate) * n);
				min_size = samples_size * ((100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100);
				max_size = samples_size * ((100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100);
				if(wanted_size < min_size) {
					wanted_size = min_size;
				} else if (wanted_size > max_size) {
					wanted_size = max_size;
				}
				if(wanted_size < samples_size) {
					/* remove samples */
					samples_size = wanted_size;
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
  return samples_size;
}