#include "VideoPicture.h"

VideoPicture::VideoPicture(void)
{
	_cond_histo = new QWaitCondition();
	_cond_video = new QWaitCondition();
	_mutex = new QMutex();

	ut = nullptr;

	last_read = 0;
	dim_codhisto = 0;
	dim_codvideo = 0;
}


VideoPicture::~VideoPicture(void)
{
}

int VideoPicture::Put(AVFrame *pFrameRGB, double pts){

	/* questa seconda lock viene invece eseguita 
	per avere un accesso mutuamente esclusivo alla lista */
	_mutex->lock();

	DataFrame p = {pFrameRGB, pts, false};
	queue.push_back(p);						//aggiungo il frame RGB alla coda

	if(dim_codvideo > 0){
		_cond_video->wakeOne();					//sveglio un processo in coda se c'è
		
	}
	else if(dim_codhisto > 0){
		_cond_histo->wakeOne();
	}

	_mutex->unlock();

	if(ut->getStopValue() == true) {
		return -1;
	}

	return 0;
}

DataFrame VideoPicture::Get3(int whois){
	
	DataFrame read;

	_mutex->lock();

	while(true){

		//CASO AVCLOCK
		if(whois == 0){

			if(!queue.empty()){

				read = queue.front();	//leggo primo elemento

				//caso seconda lettura
				if(read.read){
					queue.pop_front();
					if(last_read > 0){
						--last_read;
					}
					
				}

				//caso prima lettura
				else{
					queue.front().read = true;
				}

				break;
			}
			else {
				++dim_codvideo;
				_cond_video->wait(_mutex);
				--dim_codvideo;
			}
		}


		//CASO HISTO THREAD
		else {

			if(!queue.empty()){

				read = queue[last_read];	//leggo primo elemento
				
				//caso sono il secondo che legge il frame, lo elimino dalla coda
				if(read.read && last_read == 0){
					queue.pop_front();
				}
				else{
					queue[last_read].read = true;
				}

				if(last_read != queue.size()-1){
					++last_read;
				}
				break;
			}
			else {
				++dim_codhisto;
				_cond_histo->wait(_mutex);
				--dim_codhisto;
			}
		}
	}
	_mutex->unlock();

	return read;
}




void VideoPicture::Flush(){

	_mutex->lock();

	last_read = 0;
	/*_cond_maxsize->wakeAll();
	_cond_data->wakeAll();*/
	queue.clear();

	_mutex->unlock();
}


void VideoPicture::setUtility(Status *ut){
	this->ut = ut;
}