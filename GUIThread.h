#include <thread>
#include <string>

//QT
#include <QtOpenGL\QGLWidget>
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

//OPENGL
#include <gl\GL.h>
#include <gl\GLU.h>

//altro
#include "VideoState.h"
#include "DecodeThread.h"
#include "Refresh.h"
#include "AVClock.h"

#define FF_ALLOC_EVENT   (SDL_USEREVENT)
//Commento perché già definito in refresh #define FF_REFRESH_EVENT (SDL_USEREVENT + 1)
#define FF_QUIT_EVENT (SDL_USEREVENT + 2)

/** 
thread principale di esecuzione del video
da questo verrà poi inizializzato il thread di decodifica
*/
class GUIThread : public QThread
{
	Q_OBJECT

private:

	VideoState is;
	std::string _fileName;
	int _fullscreen;

	AVClock *_clock;									//clock

	DecodeThread *_demuxer;

	/* Variabili per OpenGL */
	GLuint _texture_video;							//come unsigned int, migliore compatibilità tra diverse piattaforme

	/**
	metodo lanciato in esecuzione per far avviare la riproduzione
	*/
	void run(void);

public:

	GUIThread(int fullscreen);

	//////////////////////////////////////////////////////////////////////

	//inizializzazione finestra SDL con dimensioni fisse 640x480
	int initializeSDL(void);

	//////////////////////////////////////////////////////////////////////

	/**
		ritorna il nome del file video
	*/
	std::string getSourceFilename();

	/**
		setta il nome del file video
	*/
	void setSourceFilename(const QString &filename);
};

