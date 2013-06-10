#include "HistoThread.h"

HistoThread::HistoThread(QObject *parent){

}

void HistoThread::run(){

	while(1){

		if(_is->ut.getStopValue()){
			break;
		}

		if(_is->ut.getPauseValue()){
			continue;
		};

		R.clear();
		G.clear();
		B.clear();

		/*
		Ridimensiono i vettori delle occorrenze e li setto a zero
		*/
		R.resize(256, 0);
		G.resize(256, 0);
		B.resize(256, 0);

		/*
			Prelevo il primo frame dalla picture queue
		*/
		cur = _is->pictq.Get3(1);

		/*
			Per ogni pixel del frame che ho prelevato calcolo le occorrenze
		*/
		int p;
		for(int y=0; y < _is->video_st->codec->height; ++y){
			for(int x=0; x < _is->video_st->codec->width; ++x){
				p = x*3+y*cur.pFrameRGB->linesize[0];		//Accedo al pixel che si trova nella riga y e colonna x
				++R[cur.pFrameRGB->data[0][p]];				//Incremento il numero di pixel rossi
				++G[cur.pFrameRGB->data[0][p+1]];			//Incremento il numero di pixel verdi
				++B[cur.pFrameRGB->data[0][p+2]];			//Incremento il numero di pixel blu
			}
		}

		_is->histo_window->setValues(R,G,B);			//Setto i valori da visualizzare

		emit ValuesReady();
	}

	qDebug() << "ESCO HISTOTHREAD";

}

void HistoThread::SetVideoState(VideoState *is){

	_is = is;
};
