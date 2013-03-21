#include "VideoPicture2.h"

VideoPicture2::VideoPicture2(void)
{
	_cond_maxsize = new QWaitCondition();
	_cond_data = new QWaitCondition();
	_mutex_data = new QMutex();
	_mutex_maxsize = new QMutex();

	quit = 0;
}


VideoPicture2::~VideoPicture2(void)
{
}

int VideoPicture2::Put(AVFrame *pFrameRGB, double pts){

	/*abbiamo bisogno che la coda non superi una determinata
	dimensione, per non dover così rallentare il processo di display a video
	nel caso la coda(buffer) fosse pieno mi metto in attesa che si liberi
	scelgo di ripartire appena si libera anche un solo posto
	*/
	_mutex_maxsize->lock();
	while(queue.size() >= VIDEO_PICTURE_QUEUE_SIZE && !quit) {
		_cond_maxsize->wait(_mutex_maxsize);	//wait for our buffer to clear
	}
	_mutex_maxsize->unlock();

	if(quit){
		return -1;
	}

	/* questa seconda lock viene invece eseguita 
	per avere un accesso mutuamente esclusivo alla lista */
	_mutex_data->lock();

	std::pair<AVFrame*, double> p(pFrameRGB, pts);
	queue.push_back(p);						//aggiungo il frame RGB alla coda

	_cond_data->wakeOne();					//sveglio un processo in coda se c'è

	_mutex_data->unlock();


	if(quit) {
		return -1;
	}

	return 0;
}

std::pair<AVFrame*, double> VideoPicture2::Get(){
	
	std::pair<AVFrame*, double> read;

	_mutex_data->lock();
	if(!queue.empty()){
		read = queue.front();	//prelevo primo elemento
		queue.pop_front();		//elimino elemento prelevato

	} else {
		_cond_data->wait(_mutex_data);
	}
	_mutex_data->unlock();


	/* una volta prelevato, vado a svegliare la coda dei frame che devono essere depositati */
	_mutex_maxsize->lock();
	_cond_maxsize->wakeOne();
	_mutex_maxsize->unlock();

	return read;
}


/* ritorna le dimensioni della coda di frame */
int VideoPicture2::getSize(void){

	return queue.size();
}