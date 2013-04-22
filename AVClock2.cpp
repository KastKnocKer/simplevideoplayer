#include "AVClock2.h"


AVClock2::AVClock2(QObject *parent) :QObject(parent)
{
	timer = new QTimer(this);
	sliderTimer = new QTimer(this);

	//inizializzazione di default del tipo di clock
	setClockType(VideoClock);

	//ogni volta che viene emesso un timeout, eseguo un video_refresh_timer
	connect(timer, &QTimer::timeout, this, &AVClock2::video_refresh_timer);
	
	/**
	ogni secondo richiamo la funzione di update dello slider
	che a sua volta emanera un segnale per aggiornare lo slider al nuovo valore
	*/
	connect(sliderTimer, &QTimer::timeout, this, &AVClock2::slider_update);
}


AVClock2::~AVClock2(void)
{
}

void AVClock2::video_refresh_timer(void){

	double actual_delay = 0.0;
	double delay = 0.0;
	double sync_threshold = 0.0;
	double ref_clock = 0.0;
	double diff = 0.0;
	double pts = 0.0;

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
			pts = p.second;
			pFrameRGB = p.first;

			_is->video_current_pts = pts;
			_is->video_current_pts_time = av_gettime();

            delay = pts - _is->frame_last_pts; /* the pts from last time */
            if (delay <= 0 || delay >= 1.0)
            {
                /* if incorrect delay, use previous one */
                delay = _is->frame_last_delay;
            }
            /* save for next time */
            _is->frame_last_delay = delay;
            _is->frame_last_pts = pts;
  
            /* update delay to sync to audio */
            ref_clock = get_audio_clock();
            diff = pts - ref_clock;

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

			//sommo il frame_timer al delay
            _is->frame_timer += delay;

			//////////////////////////////////////////////////////////////////////////////
			// comparo il risultato del calcolato frame_timer a clock del computer

            /* computer the REAL delay */
            actual_delay = _is->frame_timer - (av_gettime() / 1000000.0);

            if (actual_delay < 0.010)
            {
                /* Really it should skip the picture instead */
                actual_delay = 0.010;
            }

			//////////////////////////////////////////////////////////////////////////////

            schedule_refresh((int) (actual_delay * 1000 + 0.5));

			_is->window->setFrame(pFrameRGB);

            /* show the picture! */
            emit needupdate();

			/////////////////////////////////////////////////////////////////////////////

            /* update queue for next picture! */
            /*if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE)
            {
                is->pictq_rindex = 0;
            }
            SDL_LockMutex(is->pictq_mutex);
            is->pictq_size--;
            SDL_CondSignal(is->pictq_cond);
            SDL_UnlockMutex(is->pictq_mutex);*/

			//non devo aggiornare la coda, dato che la gestione del mutex avviene all'interno
			//della classe VideoPicture
        }
    }
    else
    {
        schedule_refresh(100);
    }

}

//////////////////////////////////////////////////////////////////////////////

double AVClock2::get_audio_clock()
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

double AVClock2::get_video_clock(){

  double delta;
  delta = (av_gettime() - _is->video_current_pts_time) / 1000000.0;
  return _is->video_current_pts + delta;
}

double AVClock2::get_external_clock(){
  return av_gettime() / 1000000.0;
}

double AVClock2::get_master_clock() {
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
void AVClock2::schedule_refresh(int delay){

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
void AVClock2::reset(void){

	timer->stop();				//fermo il timer
	setClockType(VideoClock);
	
}

/**
metodo per far partire il timer per far scorrere la barra
*/
void AVClock2::start_slider(void){

	sliderTimer->start(1000);	//il timer effettuerà un timeout ogni secondo
}

void AVClock2::slider_update(void){

	++_is->currentTime;				//aggiorno il timer 

	emit setslider(_is->currentTime);
}

///////////////////////////////////////////////////////////////////////////////

void AVClock2::SetVideoState(VideoState *is){
	_is = is;
};

VideoState* AVClock2::GetVideoState(){
	return _is;
}

QTimer* AVClock2::getTimer(){
	
	return timer;	
};

///////////////////////////////////////////////////////////////////////////////

void AVClock2::setClockType(ClockType ct)
{
    clock_type = ct;
}

AVClock2::ClockType AVClock2::clockType() const
{
    return clock_type;
}

