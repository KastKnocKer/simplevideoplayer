#include "AVClock.h"


/**
	Classe per la gestione dell'audio
*/

/**
	Metodo per decodificare i pacchetti audio e ottenre così un sample rate che andrà aggiunto alla audioq (PacketQueue)

	@param is puntatore alla classe che mantiene le informazioni sullo stato di riproduzione del video
	@param pts_ptr
	@return ritorna la dimensione del pacchetto decoficato, oppure -1 in caso di errore.
*/
int audio_decode_frame(VideoState *is, double *pts_ptr);

/**
	Metodo interno utilizzato dalla finestra SDL per aggiornare lo stato dello stream audio

	@param userdata
	@param stream
	@param len
*/
void audio_callback(void *userdata, Uint8 *stream, int len);

/**
	Metodo utilizzato in caso di VideoClock,
	l'obbiettivo è di controllare se l'audio e il video sono sincronizzati.
	Nel caso non lo fossero, toglie o aggiunge dei sample audio in questo modo
	la sincronizzazione non verrà effettuata ogni singola volta che ho una variazione.
	La chiamerò consecutivamente un numero minimo di volte.

	@param clock
	@param is
	@param samples
	@param samples_size
	@param pts
	@return Ritorna la nuova dimensione che deve avere il sample.
*/
int synchronize_audio(AVClock *clock, VideoState *is, short *samples, int samples_size, double pts);