#ifndef _Video2_H
#define _Video2_H

#include <QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <QtGui/QMouseEvent>
#include <QDebug>

//FFMPEG
extern "C"	{
	#include "dev\include\libavcodec\avcodec.h"
	#include "dev\include\libavformat\avformat.h"
	#include "dev\include\libswscale\swscale.h"
	#include "dev\include\libavutil\avutil.h"
	#undef main
}

#pragma comment(lib, "dev\\lib\\avcodec.lib")
#pragma comment(lib, "dev\\lib\\avutil.lib")
#pragma comment(lib, "dev\\lib\\avformat.lib")
#pragma comment(lib, "dev\\lib\\swscale.lib")

class QGLWidget;

/**
classe per mostrare a monitor i frame decompressi,
sfruttando OPENGL
*/
class Video2: public QGLWidget {

    Q_OBJECT

public:

    explicit Video2(QWidget *parent = 0);
	~Video2();

	//metodo per ridimensionare la finestra
	void setSize(int w, int h);

	//vado a impostare la flag in modo da permettere la riproduzione sulla finestra
	void startdisplay(void);


signals:

	//segnale emesso quando la finestra si sta chiudendo
	void windowClosing();

	//segnale emesso quando premo sul bottone per chiuedere la finestra
	void Xpressed();

public slots:
 
	//metodo richiamato a fine riproduzione per forzare la chiusura della finestra
	void closeWindow();

	void showFrame(uint8_t *data);

protected:

	/**
	metodo per inizializzazione della finestra
	viene richiamato di default alla richiesta del prima paintGL
	*/
    void initializeGL();

	//metodo per ridimensionare la finestra 
    void resizeGL(int w, int h);

	//metodo per disegnare sulla finestra
    void paintGL();

	//gestione evento pressione tasto X
    void keyPressEvent(QKeyEvent *event);

	//ridefinizione dell'evento di chiusura della finestra
	void closeEvent(QCloseEvent *event);

private:

	uint8_t *data;

	int w,h;
	GLuint _texture_Video2;

	bool display;
	bool first_frame;
	bool _extClose;

};
#endif  /* _GLWIDGET_H */
