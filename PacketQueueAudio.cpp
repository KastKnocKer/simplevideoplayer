#include "PacketQueueAudio.h"

PacketQueueAudio::PacketQueueAudio(void)
{
	this->_mutex = SDL_CreateMutex();
	this->_cond = SDL_CreateCond();

	ut = nullptr;
}


PacketQueueAudio::~PacketQueueAudio(void)
{
}

int PacketQueueAudio::Put(AVPacket* pkt){

	if(pkt != ut->getFlushPkt() && av_dup_packet(pkt)<0){
		qDebug() << "PacketQueueAudio - packet not duplicated";
		return -1;
	}

	SDL_LockMutex(_mutex);

	queue.push_back(*pkt);
														
	SDL_CondSignal(_cond);									//Invio una signal in modo da dire che c'è qualcosa prelevare al processo get
								
	SDL_UnlockMutex(_mutex);								//Esco dalla sezione critica

	return 0;
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int PacketQueueAudio::Get(AVPacket* pkt, int block){

	AVPacket prelevato;
	int ret = 0;

	//controllo se quit non vado a leggere altri pacchetti
	if(ut->getStopValue() == true){
		qDebug() << "PacketQueueVideo - rilevato QUIT";
		return -1;
	}

	SDL_LockMutex(_mutex);									//Entro nella sezione critica per accedere in modo esclusivo alla lista

	while(1){
		if(!queue.empty()) {
			prelevato = queue.front();							//ottengo il primo elemento
			queue.pop_front();									//elimino dalla lista elemento preso
			*pkt = prelevato;									//ottengo un puntatore all'oggetto prelevato
			ret = 1;
			break;
		}
		else if (!block) {										//Questo è un modo per evitare la wait, se nella chiamata di funzione si mette 1 nel parametro block nel caso non trovi 
			ret = 0;
			break;
		}
		else{													//caso lista vuota, mi metto in attesa
			SDL_CondWait(_cond, _mutex);
		}
	}
	
	SDL_UnlockMutex(_mutex);	

	return ret;
}

/**
metodo per svuotare la lista
*/
int PacketQueueAudio::Flush(){

	SDL_LockMutex(_mutex);

	queue.clear();	//svuoto la lista

	SDL_UnlockMutex(_mutex);

	return 0;
}

SDL_mutex* PacketQueueAudio::getMutex(){
	return _mutex;	
};
	
SDL_cond* PacketQueueAudio::getCond(){
	return _cond;	
};

void PacketQueueAudio::quit(){

	SDL_LockMutex(_mutex);

	queue.clear();				//Libero la coda
	SDL_CondSignal(_cond);		//Sveglio eventualmente il processo addormentato
	
	SDL_UnlockMutex(_mutex);
}

/**
metodo per settare il riferimento al parametro di quit
*/
void PacketQueueAudio::setUtility(Status *ut){
	this->ut = ut;
}
