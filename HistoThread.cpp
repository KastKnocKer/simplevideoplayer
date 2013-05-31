#include "HistoThread.h"

HistoThread::HistoThread(QObject *parent){

}

void HistoThread::run(){

	/*
		Ridimensiono i vettori delle occorrenze e li setto a zero
	*/
	R.resize(256, 0);
	G.resize(256, 0);
	B.resize(256, 0);

	while(!_is->ut.getStopValue()){
		/*
			Prelevo il primo frame dalla picture queue
		*/
		cur = _is->pictq.Get2();

		/*
			Per ogni pixel del frame che ho prelevato calcolo le occorrenze
		*/
		int p;
		for(int y=0; y < _is->video_st->codec->height; ++y){
			for(int x=0; x < _is->video_st->codec->width; ++x){
				p = x*3+y*cur.first->linesize[0];		//Accedo al pixel che si trova nella riga y e colonna x
				++R[cur.first->data[0][p]];				//Incremento il numero di pixel rossi
				++G[cur.first->data[0][p+1]];			//Incremento il numero di pixel verdi
				++B[cur.first->data[0][p+2]];			//Incremento il numero di pixel blu
			}
		}

		_is->histo_window->setValues(R,G,B);			//Setto i valori da visualizzare

		emit ValuesReady();
	}

}

void HistoThread::SetVideoState(VideoState *is){

	_is = is;
};
