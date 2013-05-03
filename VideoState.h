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

#include "SDL\include\SDL.h"
#include "SDL\include\SDL_thread.h"


/**
classe che mantiene informazioni sullo stato della riproduzione
*/
class VideoState {

private:

	//nome del video da riprodurre
	std::string      _fileName;

public:

	AVFormatContext *pFormatCtx;

	//UTILITY
	int64_t			duration;			//lunghezza del video in riproduzione
	int64_t			totalFramesNumber;	//numero totale di frame
	Status			ut;
	AVPacket		*flush_pkt;
	int				read_pause_return;


	//SEEK
	int             seek_req;			//controllo se è stato o meno richiesto il seek
	int             seek_flags;			//flag per sapere se sto facendo un forward o backward
	int64_t         seek_pos;			//posizione del seek (nuova posizione calcolata)
	int64_t			seek_rel;			//incremento nel timestamp di riproduzione

	//CLOCK
	int             av_sync_type;
	double          external_clock; /* external clock base */
	int64_t         external_clock_time;
	int				currentTime;		//tempo scorrimento slider

	//CODE
	PacketQueueAudio    audioq;
	PacketQueueVideo    videoq;
	VideoPicture		pictq;

	//AUDIO
	double			audio_clock;
	AVStream        *audio_st;
	//uint8_t         audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	uint8_t  		audio_buf[(192000*3)/2];
	unsigned int    audio_buf_size;
	unsigned int    audio_buf_index;
	AVFrame         audio_frame;
	AVPacket        audio_pkt;
	uint8_t         *audio_pkt_data;
	int             audio_pkt_size;
	int				audio_hw_buf_size;
	double			frame_timer;
	double			frame_last_pts;
	double			frame_last_delay;

	double			audio_diff_cum; /* used for AV difference average computation */
    double			audio_diff_avg_coef;
    double			audio_diff_threshold;
	int				audio_diff_avg_count;

	//VIDEO
	double			video_clock;	//pts of the last decoded frame / predicted pts of the next decoded frame
	AVStream        *video_st;		//stream del video
	uint64_t		global_video_pkt_pts;
	double			video_current_pts;	//current displayed pts (different from video_clock if frame fifos are used)
	double			video_current_pts_drift;
	int64_t			video_current_pts_time; ///time (av_gettime) at which we updated video_current_pts - used to have running video pts


	AVIOContext     *io_context;
	SwsContext		*sws_ctx;

	//STREAM
	int             videoStream, audioStream;

	//RIFERIMENTO THREAD
	QThread			*parse_tid;
	QThread			*video_tid;

	//RIFERIMENTO FINESTRA RIPRODUZIONE
	Video			*window;

	VideoState();
	~VideoState();

	std::string getSourceFilename();
	void setSourceFilename(const std::string &filename);

};

#endif //VIDEOSTATE_H