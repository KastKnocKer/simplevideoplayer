#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

//FFMPEG
extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
	#undef main
}

//SDL
#include <SDL.h>
#include <SDL_thread.h>

/**
struttura utilizzata per mantenere i pacchetti in coda
*/
typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    SDL_mutex *mutex;
    SDL_cond *cond;

} PacketQueue;

//inizializzazione della struttura
void packet_queue_init(PacketQueue *q);

//inserimento di un nuovo pacchetto
int  packet_queue_put(PacketQueue *q, AVPacket *pkt);

//ottenimento di un nuovo pacchetto
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block){

  AVPacketList *pkt1;
  int ret;
  
  SDL_LockMutex(q->mutex);
  
  for(;;) {
    
	/*if(quit){
      ret = -1;
      break;
    }*/

    pkt1 = q->first_pkt;
    if (pkt1) {
      q->first_pkt = pkt1->next;
      if (!q->first_pkt)
		q->last_pkt = NULL;
      q->nb_packets--;
      q->size -= pkt1->pkt.size;
      *pkt = pkt1->pkt;
      av_free(pkt1);
      ret = 1;
      break;
    } else if (!block) {
      ret = 0;
      break;
    } else {
      SDL_CondWait(q->cond, q->mutex);
    }
  }
  SDL_UnlockMutex(q->mutex);
  return ret;
}

#endif