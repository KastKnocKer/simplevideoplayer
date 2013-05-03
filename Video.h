#ifndef _VIDEO_H
#define _VIDEO_H

#include <QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <QtGui/QMouseEvent>
#include <QtCore>

//FFMPEG
extern "C"	{
	#include "dev\include\libavcodec\avcodec.h"
	#include "dev\include\libavformat\avformat.h"
	#include "dev\include\libswscale\swscale.h"
	#include "dev\include\libavutil\avutil.h"
}

#pragma comment(lib, "dev\\lib\\avcodec.lib")
#pragma comment(lib, "dev\\lib\\avutil.lib")
#pragma comment(lib, "dev\\lib\\avformat.lib")
#pragma comment(lib, "dev\\lib\\swscale.lib")


class Video: public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

public:

    explicit Video(QWidget *parent = 0);
	~Video();
	void setSize(int w, int h);
	void setFrame(AVFrame* pFrameRGB);

	void startdisplay(void);

signals:

	void windowClosing();	//segnale emesso quando la finestra è stata forzatamente chiusa
	void Xpressed();

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

private:

	AVFrame *pFrameRGB;
	int w,h;
	GLuint _texture_video;
	bool display;
	bool count;

	bool _extClose;

};
#endif  /* _GLWIDGET_H */
