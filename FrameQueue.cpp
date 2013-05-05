#include "FrameQueue.h"

FrameQueue::FrameQueue(void)
{
	_cond = new QWaitCondition();
	_mutex = new QMutex();


	ut = nullptr;
}


FrameQueue::~FrameQueue(void)
{
}

int FrameQueue::Put(VideoPicture vp){

	/*abbiamo bisogno che la coda non superi una determinata
	dimensione, per non dover così rallentare il processo di display a video
	nel caso la coda(buffer) fosse pieno mi metto in attesa che si liberi
	scelgo di ripartire appena si libera anche un solo posto
	*/
	//_mutex->lock();

	//while(queue.size() >= VIDEO_PICTURE_QUEUE_SIZE && (ut->getStopValue() == false)) {
	//	_cond->wait(_mutex);	//wait for our buffer to clear
	//}

	//_mutex->unlock();

	if(ut->getStopValue()){
		return -1;
	}

	/* questa seconda lock viene invece eseguita 
	per avere un accesso mutuamente esclusivo alla lista */
	_mutex->lock();

	queue.push_back(vp);						//aggiungo il frame RGB alla coda
	_cond->wakeOne();							//sveglio un processo in coda se c'è
	_mutex->unlock();

	if(ut->getStopValue()) {
		return -1;
	}

	return 0;
}

int FrameQueue::putInWindow(){

	_mutex->lock();
	data_read = queue.front().data;
	queue.pop_front();
	_mutex->unlock();

	if(ut->getStopValue()) {
		return -1;
	}

	emit show(&data_read);
	
}

int FrameQueue::Get(VideoPicture vp){
	
	VideoPicture read;

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

	return 0;
}

void FrameQueue::Flush(){

	_mutex->lock();

	/*_cond_maxsize->wakeAll();
	_cond_data->wakeAll();*/
	queue.clear();

	_mutex->unlock();
}


/* ritorna le dimensioni della coda di frame */
int FrameQueue::getSize(void){
	return queue.size();
}


void FrameQueue::setUtility(Status *ut){
	this->ut = ut;
}