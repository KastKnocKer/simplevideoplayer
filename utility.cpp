#include "utility.h"

Utility::Utility(){
	_quit = 0;
	_pause = 0;
};

Utility::~Utility(){
	
};

void Utility::setStopValue(int quit){
	_quit = quit;
};

int Utility::getStopValue(){
	
	return _quit;
};

