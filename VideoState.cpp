#include "VideoState.h"

//COSTRUTTORE
VideoState::VideoState(){

	//inizializzo la classe utility
	ut = Status();

	//ottengo il riferimento al pacchetto di FLUSH
	flush_pkt = ut.getFlushPkt();

	//inizializzo la coda di frameRGB
	pictq = VideoPicture();	
	pictq.setUtility(&ut);

	//inizializzo la coda di pacchetti audio
	audioq = PacketQueueAudio();
	audioq.setUtility(&ut);

	//inizializzo la coda di pacchetti VIDEO
	videoq = PacketQueueVideo();
	videoq.setUtility(&ut);


	pFormatCtx = NULL;
	audio_st = NULL;
	
	audio_pkt_data = NULL;
	video_st = NULL;
	io_context = NULL;
	sws_ctx = NULL;
	parse_tid = NULL;
	video_tid = NULL;

	global_video_pkt_pts = AV_NOPTS_VALUE;	//inizializzo a default il valore del PTS letto
	video_clock = 0;
	audio_clock = 0;

	audio_hw_buf_size = 0;
	frame_timer = 0;
	frame_last_pts = 0;
	frame_last_delay = 0;

	audio_diff_cum = 0; /* used for AV difference average computation */
    audio_diff_avg_coef = 0;
    audio_diff_threshold = 0;
	audio_diff_avg_count = 0;

	video_current_pts = 0.0;	//current displayed pts (different from video_clock if frame fifos are used)
	video_current_pts_drift = 0.0;
	video_current_pts_time = 0;

	av_sync_type = 0;
	external_clock = 0.0; /* external clock base */
	external_clock_time = 0;

	seek_req = 0;
	seek_flags = 0;
	seek_pos = 0;

	currentTime = 0;

	totalFramesNumber = 0;
	duration = 0;
	read_pause_return = 0;
}

//DISTRUTTORE
VideoState::~VideoState(){

}


///////////////////////////////////////////////////////////////////////////////

std::string VideoState::getSourceFilename(){

	return this->_fileName;
}

void VideoState::setSourceFilename(const std::string &filename){

	_fileName = filename;

	return;
}