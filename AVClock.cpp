#include "AVClock.h"


AVClock::AVClock(QObject *parent) :QObject(parent)
{

	timer = new QTimer(this);

	//inizializzazione di default del tipo di clock
	setClockType(AudioClock);

	//ogni volta che viene emesso un timeout, eseguo un video_refresh_timer
	connect(timer, &QTimer::timeout, this, &AVClock::video_refresh_timer);
}


AVClock::~AVClock(void)
{
}

/**
Our strategy is going to be to predict the time of the next PTS by simply measuring the time
between the previous pts and this one. At the same time, we need to sync the video to the audio.
We're going to make an audio clock: an internal value thatkeeps track of what position the audio we're playing is at.
It's like the digital readout on any mp3 player. Since we're synching the video to the audio,
the video thread uses this value to figure out if it's too far ahead or too far behind.
*/
void AVClock::video_refresh_timer(void){

	actual_delay = 0.0;
	delay = 0.0;
	sync_threshold = 0.0;
	ref_clock = 0.0;
	diff = 0.0;
	frame_current_pts = 0.0;

	//se il video_thread è attivo
	if (_is->video_st)
	{

		//se la coda è VUOTA
        if (_is->pictq.getSize() == 0)
        {
            schedule_refresh(1);
        }

		//altrimenti
        else
        {
            /* take an element from the queue */
			std::pair<AVFrame*, double> p = _is->pictq.Get();
			frame_current_pts = p.second;
			pFrameRGB = p.first;

			//setto il frame alla finestra
			_is->window->setFrame(pFrameRGB);

			_is->video_current_pts = frame_current_pts;
			_is->video_current_pts_time = av_gettime();
			_is->video_current_pts_drift = _is->video_current_pts - (_is->video_current_pts_time/1000000.0);
			
			//qDebug() << "current time: " <<  (int) _is->frame_timer;
			//qDebug() << "master clock: " << get_master_clock();
			//qDebug() << "num frame attuale: " << _is->video_st->codec->frame_number;			
			
            delay = frame_current_pts - _is->frame_last_pts; /* the pts from last time */
			//we make sure that the delay between the PTS and the previous PTS make sense
            if (delay <= 0 || delay >= 1.0)
            {
                /* if incorrect delay, use previous one */
                delay = _is->frame_last_delay;
            }
            /* save for next time */
            _is->frame_last_delay = delay;
            _is->frame_last_pts = frame_current_pts;

			/* update delay to sync to audio if not master source */
			if(clock_type != VideoClock){
  
				/* update delay to sync to audio */
				ref_clock = get_audio_clock();
				diff = frame_current_pts - ref_clock;

				/* Skip or repeat the frame. Take delay into account
				 FFPlay still doesn't "know if this is the best guess." */
				sync_threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_SYNC_THRESHOLD;

				if (fabs(diff) < AV_NOSYNC_THRESHOLD)
				{
					if (diff <= -sync_threshold)
					{
						delay = 0;
					}
					else if (diff >= sync_threshold)
					{
						delay = 2 * delay;
					}
				}
			}

			//sommo il frame_timer al delay
            _is->frame_timer += delay;

			//////////////////////////////////////////////////////////////////////////////
			// comparo il risultato del calcolato frame_timer a clock del computer

            /* computer the REAL delay */
            actual_delay = _is->frame_timer - (av_gettime() / 1000000.0);

			//qDebug() << "delay: " << delay << "actual delay: " << actual_delay << "frame current pts: " << frame_current_pts << "A-V: " << -diff;

            if (actual_delay < 0.010)
            {
                /* Really it should skip the picture instead */
                actual_delay = 0.010;
            }

			//////////////////////////////////////////////////////////////////////////////

			schedule_refresh((int) (actual_delay * 1000 + 0.5));

            /* show the picture and update the timer*/
            emit needupdate();
			
        }
    }
    else
    {
        schedule_refresh(100);
    }

	return;

}

//////////////////////////////////////////////////////////////////////////////

double AVClock::get_audio_clock()
{
    double audio_pts;
    int hw_buf_size, bytes_per_sec, n;

    audio_pts = _is->audio_clock; /* maintained in the audio thread */
    hw_buf_size = _is->audio_buf_size - _is->audio_buf_index;
    bytes_per_sec = 0;

	//se attivo il stream di decodifica audio
    if (_is->audio_st)
    {
        n = _is->audio_st->codec->channels * 2;
        bytes_per_sec = _is->audio_st->codec->sample_rate * n;
    }
    if (bytes_per_sec)
    {
        audio_pts -= (double) hw_buf_size / bytes_per_sec;
    }
    return audio_pts;
}

/**
come calcoliamo il video_clock
video clock = PTS_of_last_read_frame + (current_time - time_elapsed_since_PTS_value_was_get)
*/
double AVClock::get_video_clock(){
	
	if(_is->ut.getPauseValue() == true){
		return _is->video_current_pts;
	} else {
		return _is->video_current_pts_drift + av_gettime()/1000000.0;	
	}
}


double AVClock::get_external_clock(){
  return av_gettime() / 1000000.0;
}

/**
	metodo che ritorna il clock, in base al tipo di sincronizzazione impostato
*/
double AVClock::get_master_clock() {
	if(clock_type == VideoClock) {
		return get_video_clock();
  } else if(clock_type == AudioClock) {
		return get_audio_clock();
  } else {
		return get_external_clock();
  }
}

//////////////////////////////////////////////////////////////////////////////

/**
funzione che va a richiamare dopo "delay" millisecondi
il refresh della pagina
*/
void AVClock::schedule_refresh(int delay){

	/**
	faccio partire un timer della durata di "delay" millisecondi
	al termine viene emesso di default un segnale "timeout",
	a questo verrà associato lo slot video_refresh_timer
	*/
	timer->start(delay);
}

/**
SLOT
utilizzato per resettare il clock a termine riproduzione
*/
void AVClock::reset(void){

	timer->stop();				//fermo il timer
}

///////////////////////////////////////////////////////////////////////////////

void AVClock::SetVideoState(VideoState *is){
	_is = is;
};

VideoState* AVClock::GetVideoState(){
	return _is;
}

QTimer* AVClock::getTimer(){
	return timer;	
};

///////////////////////////////////////////////////////////////////////////////

void AVClock::setClockType(ClockType ct){
    clock_type = ct;
}

AVClock::ClockType AVClock::clockType() const{
    return clock_type;
}

