#include "VideoState.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
/* AUDIO */
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
metodo per decodificare i pacchetti audio e ottenre cosi un sample rate che andrà
aggiunto alla audioq (PacketQueue)
*/
int audio_decode_frame(VideoState *is, double *pts_ptr);

/**
metodo interno utilizzato dalla finestra SDL per aggiornare lo stato dello stream audio
*/
void audio_callback(void *userdata, Uint8 *stream, int len);