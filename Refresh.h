
#include "VideoState.h"
#include "StaticValue.h"

static Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque){

	SDL_Event event;
	event.type = FF_REFRESH_EVENT;
	event.user.data1 = opaque;
	SDL_PushEvent(&event);
	return 0; /* 0 means stop timer */
	//quando torna 0, SDL ferma il timer, in questo modo la callback non viene ripetuta

}

/* schedule a video refresh in 'delay' ms */
static void schedule_refresh(VideoState *is, int delay) {
	SDL_AddTimer(delay, sdl_refresh_timer_cb, is);	//aggiungo un timer che andrà a chiamare la funzione di refresh della schermata
}

/**
cosa fa questa funzione:
- preleva un frame dalla picture_queue quando ne trova uno
- set del timer su quando dovrà essere visualizzata la nuova picture
- chiama video_display per mostrare il frame prelevato a schermo
*/
void video_refresh_timer(void *userdata);

/**
funzione per mostrare a video il frame
è in questa funzione che andremo a modificare valori della finestra per un resize
*/
void video_display(VideoState *is);