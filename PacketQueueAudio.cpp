#include "PacketQueueAudio.h"

PacketQueueAudio::PacketQueueAudio(Utility *manager)
{

	PacketQueueAudio::manager = manager;
	this->_mutex = SDL_CreateMutex();
	this->_cond = SDL_CreateCond();
}

PacketQueueAudio::PacketQueueAudio(void)
{
	manager = nullptr;
	this->_mutex = SDL_CreateMutex();
	this->_cond = SDL_CreateCond();
}


PacketQueueAudio::~PacketQueueAudio(void)
{
}

int PacketQueueAudio::Put(AVPacket* pkt){

	SDL_LockMutex(_mutex);

	queue.push_back(*pkt);
														
	SDL_CondSignal(_cond);									//Invio una signal in modo da dire che c'è qualcosa prelevare al processo get
								
	SDL_UnlockMutex(_mutex);								//Esco dalla sezione critica

	return 0;
}

int PacketQueueAudio::Get(AVPacket* pkt, int block){

	SDL_LockMutex(_mutex);														//Entro nella sezione critica per accedere in modo esclusivo alla lista

	if(!queue.empty()) {
		*pkt = queue.front();										//ottengo il primo elemento
		queue.pop_front();												//elimino dalla lista elemento preso
	}
	else if (!block) {													//Questo è un modo per evitare la wait, se nella chiamata di funzione si mette 1 nel parametro block nel caso non trovi 
		return -1;
	}
	else{																//caso lista vuota, mi metto in attesa
		SDL_CondWait(_cond, _mutex);
	}
	
	SDL_UnlockMutex(_mutex);	

	return 0;
}

/**
ritorna la dimensione della lista
*/
int PacketQueueAudio::getSize(){

	return queue.size();

}

SDL_mutex* PacketQueueAudio::getMutex(){
	return _mutex;	
};
	
SDL_cond* PacketQueueAudio::getCond(){
	return _cond;	
};

void PacketQueueAudio::quit(){
	SDL_CondSignal(_cond);		//Sveglio eventualmente il processo addormentato
	queue.clear();				//Libero la coda
}