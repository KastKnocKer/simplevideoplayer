#ifndef VIDEOSTATE_H
#define VIDEOSTATE_H

#include <QDebug>
#include <QtOpenGL\QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>

#include <string>

#include "PacketQueueAudio.h"
#include "PacketQueueVideo.h"
#include "VideoPicture.h"
#include "Video.h"
#include "HistoDraw.h"

#include "SDL\include\SDL.h"
#include "SDL\include\SDL_thread.h"
#undef main


/**
	Classe che mantiene informazioni sullo stato della riproduzione
	@author Matteo Renzi
*/
class VideoState {

private:

	std::string      _fileName;					/* Path del video da riprodurre */

public:

	AVFormatContext *pFormatCtx;				/* Contiene le informazioni sullo stream, come ad esempio il tipo di codec, etc. */

	//UTILITY
	int64_t			duration;					/* Lunghezza del video in riproduzione */
	int64_t			totalFramesNumber;			/* Numero totale di frame */
	Status			ut;							/* puntatore alla classe stato riproduzione */
	AVPacket		*flush_pkt;					/* pacchetto di flush utilizzato in caso di seek */
	int				read_pause_return;			/*  */


	//SEEK
	int             seek_req;					/* Controllo se è stato o meno richiesto il seek */
	int             seek_flags;					/* Flag per sapere se sto facendo un forward o backward */
	int64_t         seek_pos;					/* Posizione del seek (nuova posizione calcolata) */
	int64_t			seek_rel;					/* Incremento nel timestamp di riproduzione */

	//CLOCK
	int             av_sync_type;				/*  */
	double          external_clock;				/* External clock base */
	int64_t         external_clock_time;		/*  */
	int				currentTime;				/* Tempo scorrimento slider */

	//CODE
	PacketQueueAudio    audioq;					/* Coda per i pacchetti audio */
	PacketQueueVideo    videoq;					/* Coda per i pacchetti video */
	VideoPicture		pictq;					/* Coda per i pacchetti video RGB con il relativo PTS */

	//AUDIO
	double			audio_clock;				/*  */
	AVStream        *audio_st;					/* puntatore allo stream audio */
	uint8_t         audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];	/*  */
	unsigned int    audio_buf_size;				/* Dimensione buffer audio */
	unsigned int    audio_buf_index;			/*  */
	AVFrame         audio_frame;				/* Frame audio corrente */
	AVPacket        audio_pkt;					/* Pacchetto audio corrente */
	uint8_t         *audio_pkt_data;			/* Dati contenuti nel pacchetto audio corrente */
	int             audio_pkt_size;				/* Dimensione pacchetto audio corrente */
	int				audio_hw_buf_size;			/* Dimensione del buffer audio hardware */
	double			frame_timer;				/* tempo trascorso dall'ultimo frame */
	double			frame_last_pts;				/* PTS ultimo frame */
	double			frame_last_delay;			/* delay tra ultimo frame e adesso, utilizzato per PAUSE */

	double			audio_diff_cum;				/* Used for AV difference average computation */
    double			audio_diff_avg_coef;		/*  */
    double			audio_diff_threshold;		/*  */
	int				audio_diff_avg_count;		/*  */

	//VIDEO
	double			video_clock;				/* pts of the last decoded frame / predicted pts of the next decoded frame */
	AVStream        *video_st;					/* Stream del video */
	uint64_t		global_video_pkt_pts;		/*  */
	double			video_current_pts;			/* Current displayed pts (different from video_clock if frame fifos are used) */
	double			video_current_pts_drift;	/*  */
	int64_t			video_current_pts_time;		/* time (av_gettime) at which we updated video_current_pts - used to have running video pts */


	AVIOContext     *io_context;				/* Context AVIO */
	SwsContext		*sws_ctx;					/* Context SWS */

	//STREAM
	int             videoStream;				/* indice di stream video */
	int				audioStream;				/* indice di stream audio */

	//RIFERIMENTO THREAD
	QThread			*parse_tid;					/* rifetimento al thread di decodifica */
	QThread			*video_tid;					/* riferimento al thread video */

	//RIFERIMENTO FINESTRA RIPRODUZIONE
	Video			*window;					/* puntatore alla finestra di riproduzione */

	//RIFERIMENTO FINESTRA PER DISEGNARE L'ISTOGRAMMA
	HistoDraw *histo_window;

	/**
		Costruttore
	*/
	VideoState();

	/**
		Distruttore
	*/
	~VideoState();

	/**
		Metodo per ottenere il path del video in riproduzione
		@return path video aperto.
	*/
	std::string getSourceFilename();

	/**
		Metodo per settare il path del video in riproduzione
		@param filename path del video aperto.
	*/
	void setSourceFilename(const std::string &filename);

};

#endif //VIDEOSTATE_H