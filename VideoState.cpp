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