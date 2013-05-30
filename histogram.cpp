#include "histogram.h"

histogram::histogram(QObject *parent){
}

void histogram::run(){
	/*
		Ridimensiono i vettori delle occorrenze e li setto a zero
	*/
	R.resize(256, 0);
	G.resize(256, 0);
	B.resize(256, 0);

	/*
		Prelevo il primo frame dalla picture queue
	*/
	cur = _is->pictq.Get2();
	qDebug() << "Prelevo elemento";

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

	qDebug() << "R";
	for(unsigned i=0; i<R.size(); ++i){
		qDebug() << R[i];
	}
	qDebug() << "end";

}

void histogram::SetVideoState(VideoState *is){

	_is = is;
};
