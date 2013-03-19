#ifndef VIDEOSTATE_H
#define VIDEOSTATE_H

#include <QDebug>
#include <QtOpenGL\QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>

#include <string>

#include "PacketQueueAudio.h"
#include "PacketQueueVideo.h"
#include "VideoPicture2.h"
#include "Video.h"

#include <SDL.h>
#include <SDL_thread.h>


/**
classe che mantiene informazioni sullo stato della riproduzione
*/
class VideoState {

private:

	std::string      _fileName;

public:

	AVFormatContext *pFormatCtx;

	//AUDIO
	double			audio_clock;
	AVStream        *audio_st;
	PacketQueueAudio    audioq;
	uint8_t         audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
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

	//VIDEO
	double			video_clock;	//pts of the last decoded frame / predicted pts of the next decoded frame
	AVStream        *video_st;
	PacketQueueVideo    videoq;
	VideoPicture2	pictq;
	uint64_t		global_video_pkt_pts;

	int             quit;

	AVIOContext     *io_context;
	SwsContext		*sws_ctx;

	int             videoStream, audioStream;

	QThread			*parse_tid;
	QThread			*video_tid;

	Video			*window;

	VideoState();
	~VideoState();

	std::string getSourceFilename();
	void setSourceFilename(const std::string &filename);

	void setGlobalVideoState(VideoState *);
	VideoState* getGlobalVideoState();
};

#endif //VIDEOSTATE_H