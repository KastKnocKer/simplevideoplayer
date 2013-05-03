#include "PacketQueueVideo.h"

PacketQueueVideo::PacketQueueVideo(void)
{
	_mutex = new QMutex();
	_cond = new QWaitCondition();

	ut = nullptr;
}


PacketQueueVideo::~PacketQueueVideo(void)
{
}

int PacketQueueVideo::Put(AVPacket *pkt){

	/* duplicate the packet */
	if(pkt != ut->getFlushPkt() && av_dup_packet(pkt)<0){
		qDebug() << "PacketQueueVideo - packet not duplicated";
		return -1;
	}

	_mutex->lock();

	queue.push_back(*pkt);
														
	_cond->wakeOne();									//Invio una signal in modo da dire che c'è qualcosa prelevare al processo get
								
	_mutex->unlock();									//Esco dalla sezione critica

	return 0;
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int PacketQueueVideo::Get(AVPacket *pkt, int block){

	AVPacket prelevato;
	int ret = 0;

	//controllo se quit non vado a leggere altri pacchetti
	if(ut->getStopValue() == true){
		qDebug() << "PacketQueueVideo - rilevato QUIT";
		return -1;
	}
																				
	_mutex->lock();														//Entro nella sezione critica per accedere in modo esclusivo alla lista

	if(!queue.empty()) {
		prelevato = queue.front();										//ottengo il primo elemento
		queue.pop_front();												//elimino dalla lista elemento preso
		*pkt = prelevato;												//ottengo un puntatore all'oggetto prelevato
		ret = 1;
	}
	//else if(ut->getEOFValue() == true){
	//	/* se la coda è vuota, controllo il flag di eof, se è true
	//	allora abbiamo letto e visualizzato tutti i pachetti. Allora
	//	solo in questo momento devo settare quit*/
	//	ut->setStopValue(true);
	//	ret = 0;
	//}
	else if (!block) {													//Questo è un modo per evitare la wait, se nella chiamata di funzione si mette 1 nel parametro block nel caso non trovi 
		ret = 0;
	}
	else{																//caso lista vuota, mi metto in attesa
		_cond->wait(_mutex);
	}
	
	_mutex->unlock();

	return ret;
}

/**
metodo per svuotare la lista
*/
int PacketQueueVideo::Flush(){

	_mutex->lock();

	queue.clear();	//svuoto la lista

	_mutex->unlock();

	return 0;
}

/**
ritorna la dimensione della lista
*/
int PacketQueueVideo::getSize(){

	return queue.size();

}

QMutex* PacketQueueVideo::getMutex(){
	return _mutex;	
};
	
QWaitCondition* PacketQueueVideo::getCond(){
	return _cond;	
};

void PacketQueueVideo::quit(){

	_mutex->lock();

	queue.clear();			//Svuoto la coda
	_cond->wakeAll();		//Sveglio tutti i processi che sono eventualmente in coda
	
	_mutex->unlock();
}

/**
metodo per settare il riferimento al parametro di quit
*/
void PacketQueueVideo::setUtility(Status *ut){
	this->ut = ut;
}
