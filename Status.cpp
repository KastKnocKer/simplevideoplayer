#include "Status.h"

Status::Status(){

	_quit = false;
	_pause = false;
	_lastpause = false;
	_eof = false;

	av_init_packet(&flush_pkt);
	flush_pkt.data = (unsigned char *) "FLUSH";
};

Status::~Status(){
	av_free_packet(&flush_pkt);
};

////////////////////////////////////////////////////////

void Status::setStopValue(bool quit){
	_quit = quit;
};



////////////////////////////////////////////////////////

void Status::setPauseValue(bool pause){
	_pause = pause;
};



////////////////////////////////////////////////////////

void Status::setLastPauseValue(bool lastpause){
	_lastpause = lastpause;
};


////////////////////////////////////////////////////////

void Status::setEOFValue(bool eof){
	_eof = eof;
}

////////////////////////////////////////////////////////

AVPacket* Status::getFlushPkt(){
	return &flush_pkt;
}


