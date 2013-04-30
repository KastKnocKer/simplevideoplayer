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

bool Status::getStopValue(){
	return this->_quit;
};

////////////////////////////////////////////////////////

void Status::setPauseValue(bool pause){
	_pause = pause;
};

bool Status::getPauseValue(){	
	return this->_pause;
};

////////////////////////////////////////////////////////

void Status::setLastPauseValue(bool lastpause){
	_lastpause = lastpause;
};

bool Status::getLastPauseValue(){	
	return this->_lastpause;
};

bool Status::isPauseChanged(){
	if(_pause != _lastpause){
		return true;
	} else {
		return false;
	}
}

////////////////////////////////////////////////////////

void Status::setEOFValue(bool eof){
	_eof = eof;
}
	
bool Status::getEOFValue(){
	return _eof;
}

////////////////////////////////////////////////////////

AVPacket* Status::getFlushPkt(){
	return &flush_pkt;
}


