#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H

#ifndef INT64_C
#define INT64_C(c) (int64_t)(c)
#define UINT64_C(c) (uint64_t)(c)
#endif

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <atomic>

//QT
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QtOpenGL\QGLWidget>

//OPENGL
#include <gl\GL.h>
#include <gl\GLU.h>

#include "PacketQueue2.h"
#include "VideoState.h"

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
#include <SDL_video.h>

/* prevents SDL from ovverriding main() */
#ifndef __MINGW32__
#undef main														
#endif

const PixelFormat CONV_FORMAT = PIX_FMT_RGB24;	//definisco una costante che rapresenta il formato dei pixel voluto
#define SDL_AUDIO_BUFFER_SIZE 1024

void audio_callback_wrapper(void *userdata, Uint8 *stream, int len);


/*
	thread per la decompressione del video
	@param nome del video da decomprimere
*/
class DecompressionThread{

private:

	//variabili che mantengono il nome del file scelto per la riproduzione
	std::string _fileName;
	const char* _video;
	
	//FFMPEG
	/* video */
	int             videoStream;
	int				fullscreen;
	AVFormatContext *_pFormatCtx;					//formato del video
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	/* frame video YVU e RGB*/
	AVFrame         *pFrame, *pFrameRGB;
	AVPacket        packet;
	AVDictionary	*videoOptDict;
	int             frameFinished;					//variabile di controllo per sapere se ho finito di caricare il frame

	/* audio */
	int				audioStream;					//indice per lo stream audio
	AVCodecContext	*aCodecCtx;
	AVCodec			*aCodec;
	AVDictionary	*audioOptDict;

	int				bytes;							//utility per conversione RGB, mantengo segnato lo spazio occupato dal frame convertito in RGB
	struct SwsContext	  *pSWSContext;

	/* Variabili per SDL */
	SDL_Overlay     *bmp;
	SDL_Surface     *screen;
	SDL_Rect        rect;
	SDL_Event       event;
	AVPicture		pict;

	SDL_AudioSpec	wanted_spec, spec;
	PacketQueue2 audioq;

	/* Variabili per OpenGL */
	GLuint _texture_video;							//come unsigned int, migliore compatibilità tra diverse piattaforme

	uint16_t width;
	uint16_t height;

	VideoState *is;

public:

	std::thread _thread;							//thread
	std::mutex _mutex;

	//COSTRUTTORE
	DecompressionThread();

	//DISTRUTTORE
	~DecompressionThread();

	//metodo per far partire il thread
	void start();

	//metodo che esegue il codice di decompressione in esecuzione
	void run();

	//metodo per inizializzare il decodificatore
	void initializeFFMPEG();

	//metodo per inizializzare la finestra SDL
	void initializeSDL();

	//inizializzo OpenGL
	void initializeGL();

	//metodo che esegue la un ciclo di draw sulla finestra openGL
	//fintanto che trova dei frame
	void next_frame();

	/**
		ritorna il nome del file video
	*/
	std::string getSourceFilename();

	/**
		setta il nome del file video
	*/
	void setSourceFilename(const QString &filename);

	//void SetManager(Utility manager);
	//Utility* GetManager();

	//void audio_callback(void *userdata, Uint8 *stream, int len);

	int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size);

	/**
	metodo per settare la variabile globale decompressionThread
	*/
	void set(DecompressionThread *t);


};

#endif //DECOMPRESSION_H