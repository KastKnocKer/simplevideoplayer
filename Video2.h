#pragma once

#include <QGLWidget>
#include <QImage>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <QtGui/QMouseEvent>

//FFMPEG
extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
}

class Video2: public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:
    Video2(QWidget *parent = NULL);
	~Video2();
	void setSize(int w, int h);
	void setImg(QImage img);

	void startdisplay(void);

protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

	QImage img;
	int w,h;
	GLuint _texture_video;
	bool display;
	bool count;

};
