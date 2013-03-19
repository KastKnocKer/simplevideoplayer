#include "Refresh.h"

void video_refresh_timer(void *userdata) {

	VideoState *is = (VideoState *) userdata;
	// vp is used in later tutorials for synchronization
	//VideoPicture *vp;
  
	if(is->video_st) {

		/* controllo se la coda dei frame è VUOTA */
		if(is->pictq.getSize() == 0) {

			schedule_refresh(is, 1);	//richiedo un altro resfresh dopo 1ms

		/* se la coda è PIENA */
		} else {
			//vp = &is->pictq[is->pictq_rindex];
			/* Now, normally here goes a ton of code
			about timing, etc. we're just going to
			guess at a delay for now. You can
			increase and decrease this value and hard code
			the timing - but I don't suggest that ;)
			We'll learn how to do it for real later.
			*/
			schedule_refresh(is, 80);	//richiedo un altro resfresh dopo 80ms
      
			/* show the picture! */
			video_display(is);
      
			/* update queue for next picture! */
			/*if(++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) {
				is->pictq_rindex = 0;
			}*/

			//secondo me non sveglio piu qua il processo in attesa di scrivere su queue, dato che lo sveglio al termine del GET
			/*SDL_LockMutex(is->pictq_mutex);
				is->pictq_size--;
				SDL_CondSignal(is->pictq_cond);
			SDL_UnlockMutex(is->pictq_mutex);*/
		}
	} 
	else {

		schedule_refresh(is, 100);

	}
}

////////////////////////////////////////////////////////////////////////////////////

/* metodo per mostrare a schermo immagine */
void video_display(VideoState *is){

	AVFrame img;

	SDL_Rect rect;
	//AVPicture pict;
	float aspect_ratio;
	int w, h, x, y;
	//int i;

	img = is->pictq.Get();

	glBindTexture( GL_TEXTURE_2D, is->texture_video );									//associo la texture corrente						

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, is->video_st->codec->width, is->video_st->codec->height, GL_RGB, GL_UNSIGNED_BYTE, img.data[0]);

	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture( GL_TEXTURE_2D, is->texture_video );
	glScalef(1.0f, -1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f); 
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glScalef(1.0f, -1.0f, 1.0f);

	SDL_GL_SwapBuffers();

	/*if(vp->bmp) {
		if(is->video_st->codec->sample_aspect_ratio.num == 0) {
		aspect_ratio = 0;
		} 
		else {
			aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio) *
			is->video_st->codec->width / is->video_st->codec->height;
		}

		if(aspect_ratio <= 0.0) {
			aspect_ratio = (float)is->video_st->codec->width / (float)is->video_st->codec->height;
		}


		h = screen->h;
		w = ((int)rint(h * aspect_ratio)) & -3;
		if(w > screen->w) {
			w = screen->w;
			h = ((int)rint(w / aspect_ratio)) & -3;
		}
		x = (screen->w - w) / 2;
		y = (screen->h - h) / 2;
    
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_DisplayYUVOverlay(vp->bmp, &rect);
	}*/

}

