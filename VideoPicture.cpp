#include "VideoPicture.h"

int queue_picture(VideoState *is, AVFrame *pFrameRGB){

	/* 
	per utilizzare questa coda abbiamo necessità di 2 puntatori:
	-uno all'indice di lettura
	-uno all'indice di scrittura
	sono mantenuti dentro VideoState
	*/
	VideoPicture *vp;
	AVPicture pict;

	SDL_LockMutex(is->pictq_mutex);

	while(is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE && !is->quit) {
		SDL_CondWait(is->pictq_cond, is->pictq_mutex);	//wait for our buffer to clear
	}

	SDL_UnlockMutex(is->pictq_mutex);

	if(is->quit)
		return -1;

	// windex is set to 0 initially
	vp = &is->pictq[is->pictq_windex];	//vado a ottenere il puntatore alla VideoPicture nella picture_queue all'indice pictq_windex

	/* allocate or resize the buffer! */
	if(!vp->bmp || vp->width != is->video_st->codec->width || vp->height != is->video_st->codec->height) {
		
		SDL_Event event;

		vp->allocated = 0;
		/* we have to do it in the main thread */
		event.type = FF_ALLOC_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);

		/* wait until we have a picture allocated */
		SDL_LockMutex(is->pictq_mutex);
		while(!vp->allocated && !is->quit) {
			SDL_CondWait(is->pictq_cond, is->pictq_mutex);
		}
		SDL_UnlockMutex(is->pictq_mutex);
		if(is->quit) {
			return -1;
		}
	}

	/* We have a place to put our picture on the queue */
	if(vp->bmp) {

		SDL_LockYUVOverlay(vp->bmp);
    
		/* point pict at the queue */

		pict.data[0] = vp->bmp->pixels[0];
		pict.data[1] = vp->bmp->pixels[2];
		pict.data[2] = vp->bmp->pixels[1];
    
		pict.linesize[0] = vp->bmp->pitches[0];
		pict.linesize[1] = vp->bmp->pitches[2];
		pict.linesize[2] = vp->bmp->pitches[1];
    
		// Convert the image into YUV format that SDL uses
		sws_scale
		(
			is->sws_ctx,
			(uint8_t const * const *)pFrame->data,
			pFrame->linesize,
			0, 
			is->video_st->codec->height, 
			pict.data, 
			pict.linesize
		);
    
		SDL_UnlockYUVOverlay(vp->bmp);

		/* now we inform our display thread that we have a pic ready */
		if(++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) {
			is->pictq_windex = 0;
		}

		SDL_LockMutex(is->pictq_mutex);
		++is->pictq_size;									//aumento la dimensione della coda
		SDL_UnlockMutex(is->pictq_mutex);
	}

	return 0;

};


/////////////////////////////////////////////////////////////////////////////////////////////////


void alloc_picture(void *userdata){

	VideoState *is = (VideoState *)userdata;
	VideoPicture *vp;

	vp = &is->pictq[is->pictq_windex];
	if(vp->bmp) {
		// we already have one make another, bigger/smaller
		SDL_FreeYUVOverlay(vp->bmp);
	}

	
	// Allocate a place to put our YUV image on that screen
	vp->bmp = SDL_CreateYUVOverlay(is->video_st->codec->width,
				 is->video_st->codec->height,
				 SDL_YV12_OVERLAY,
				 screen);
	vp->width = is->video_st->codec->width;
	vp->height = is->video_st->codec->height;
  
	SDL_LockMutex(is->pictq_mutex);
	vp->allocated = 1;
	SDL_CondSignal(is->pictq_cond);
	SDL_UnlockMutex(is->pictq_mutex);

}