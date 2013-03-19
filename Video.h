#ifndef _VIDEO_H
#define _VIDEO_H

#include <QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <QtGui/QMouseEvent>

//FFMPEG
extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
	#undef main
}

class Video: public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    Video(QWidget *parent = NULL);
	~Video();
	void setSize(int w, int h);
	void setFrame(AVFrame* pFrameRGB);

	void startdisplay(void);

protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

	AVFrame *pFrameRGB;
	int w,h;
	GLuint _texture_video;
	bool display;
	bool count;

};
#endif  /* _GLWIDGET_H */
