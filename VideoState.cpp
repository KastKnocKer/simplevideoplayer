#include "VideoState.h"

VideoState* global_video_state;

//COSTRUTTORE
VideoState::VideoState(){

	quit = 0;
	pictq = VideoPicture2();	//inizializzo la coda di frameRGB

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

	video_current_pts = 0;	//current displayed pts (different from video_clock if frame fifos are used)
	video_current_pts_time = 0;

	av_sync_type = 0;
	external_clock = 0.0; /* external clock base */
	external_clock_time = 0;

	seek_req = 0;
	seek_flags = 0;
	seek_pos = 0;

	av_init_packet(&flush_pkt);
	flush_pkt.data = (unsigned char *) "FLUSH";

	currentTime = 0;

	pause = false;

	totalFramesNumber = 0;
	duration = 0;
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


////////////////////////////////////////////////////////////////////////////////

/**
metodo per settare il puntatore globale allo stato del video
*/
void VideoState::setGlobalVideoState(VideoState *is){

	global_video_state = is;
}

VideoState* VideoState::getGlobalVideoState(){

	return global_video_state;

}

////////////////////////////////////////////////////////////////////////////////