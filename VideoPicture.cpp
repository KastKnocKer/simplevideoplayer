#include "VideoPicture.h"

VideoPicture::VideoPicture(void)
{
	_cond = new QWaitCondition();
	_mutex = new QMutex();

	ut = nullptr;
}


VideoPicture::~VideoPicture(void)
{
}

int VideoPicture::Put(AVFrame *pFrameRGB, double pts){

	/* questa seconda lock viene invece eseguita 
	per avere un accesso mutuamente esclusivo alla lista */
	_mutex->lock();

	std::pair<AVFrame*, double> p(pFrameRGB, pts);
	queue.push_back(p);						//aggiungo il frame RGB alla coda

	_cond->wakeOne();					//sveglio un processo in coda se c'è

	_mutex->unlock();

	if(ut->getStopValue() == true) {
		return -1;
	}

	return 0;
}

std::pair<AVFrame*, double> VideoPicture::Get(){
	
	std::pair<AVFrame*, double> read;

	_mutex->lock();

	while(true){
		if(!queue.empty()){
			read = queue.front();	//prelevo primo elemento
			queue.pop_front();		//elimino elemento prelevato
			break;
		} else {
			_cond->wait(_mutex);
		}
	}
	_mutex->unlock();

	return read;
}

void VideoPicture::Flush(){

	_mutex->lock();

	/*_cond_maxsize->wakeAll();
	_cond_data->wakeAll();*/
	queue.clear();

	_mutex->unlock();
}


void VideoPicture::setUtility(Status *ut){
	this->ut = ut;
}