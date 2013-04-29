#ifndef _VIDEO_H
#define _VIDEO_H

#include <QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <QtGui/QMouseEvent>
#include <QtCore>

//FFMPEG
extern "C"	{
	#include "ffmpeg\include\libavcodec\avcodec.h"
	#include "ffmpeg\include\libavformat\avformat.h"
	#include "ffmpeg\include\libswscale\swscale.h"
	#include "ffmpeg\include\libavutil\avutil.h"
}

class Video: public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:

    Video(QWidget *parent = NULL);
	~Video();
	void setSize(int w, int h);
	void setFrame(AVFrame* pFrameRGB);

	void startdisplay(void);

public slots:

	/** 
	metodo richiamato a fine riproduzione per forzare la chiusura della finestra
	*/
	void closeWindow();

protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);

signals:

	void windowClosing();	//segnale emesso quando la finestra è stata forzatamente chiusa

private:

	AVFrame *pFrameRGB;
	int w,h;
	GLuint _texture_video;
	bool display;
	bool count;

};
#endif  /* _GLWIDGET_H */
