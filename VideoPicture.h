
#include <string>
#include <QDebug>

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

#include "VideoState.h"

#define FF_ALLOC_EVENT   (SDL_USEREVENT)

typedef struct VideoPicture {
  SDL_Overlay *bmp;
  int width, height; /* source height & width */
  int allocated;
} VideoPicture;

int queue_picture(VideoState *is, AVFrame *pFrameRGB);

/* metodo per allocare una Picture */
void alloc_picture(void *userdata);