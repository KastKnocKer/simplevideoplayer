#ifndef _VIDEO_H
#define _VIDEO_H

#include <QGLWidget>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <QtGui/QMouseEvent>

//FFMPEG
extern "C"	{
	#include "dev\include\libavcodec\avcodec.h"
	#include "dev\include\libavformat\avformat.h"
	#include "dev\include\libswscale\swscale.h"
	#include "dev\include\libavutil\avutil.h"
	#include "dev\include\libavutil\pixdesc.h"
	#undef main
}

#pragma comment(lib, "dev\\lib\\avcodec.lib")
#pragma comment(lib, "dev\\lib\\avutil.lib")
#pragma comment(lib, "dev\\lib\\avformat.lib")
#pragma comment(lib, "dev\\lib\\swscale.lib")

class QGLWidget;

/**
	Classe per mostrare a monitor i frame decompressi,
	sfruttando OPENGL, basata su una struttura di finestra OpenGL predefinita di Qt.
	@author Giovanni Esposito
*/
class Video: public QGLWidget {

    Q_OBJECT

public:

	/**
		Costruttore
		@param parent
	*/
    explicit Video(QWidget *parent = 0);

	/**
		Distruttore
	*/
	~Video();

	/**
		Metodo per ridimensionare la finestra
		@param w larghezza
		@param h altezza
	*/
	void setSize(int w, int h);

	/**
		Metodo per impostare il frame corrente alla finestra
		@param pFrameRGB frame da mostrare
	*/
	void setFrame(AVFrame* pFrameRGB);

	/**
		Vado a impostare la flag in modo da permettere la riproduzione sulla finestra
	*/
	void startdisplay(void);


signals:

	/**
		Segnale emesso quando la finestra si sta chiudendo
	*/
	void windowClosing();

	/**
		Segnale emesso quando premo sul bottone per chiuedere la finestra
	*/
	void Xpressed();

public slots:
 
	/**
		Metodo richiamato a fine riproduzione per forzare la chiusura della finestra
	*/
	void closeWindow();

protected:

	/**
		Metodo per inizializzazione della finestra.
		Viene richiamato di default alla richiesta del prima paintGL
	*/
    void initializeGL();

	/**
		Metodo per ridimensionare la finestra 
		@param w larghezza
		@param h altezza
	*/
    void resizeGL(int w, int h);

	/**
		Metodo per disegnare sulla finestra
	*/
    void paintGL();

	/**
		Gestione evento pressione tasto X
		@param event evento pressione tasto
	*/
    void keyPressEvent(QKeyEvent *event);

	/**
		Ridefinizione dell'evento di chiusura della finestra
		@param event evento chiusura finestra
	*/
	void closeEvent(QCloseEvent *event);

private:

	AVFrame *pFrameRGB;		/* Frame da mostrare a video */
	int w;					/* Larghezza finestra */
	int h;					/* Altezza finestra */
	GLuint _texture_video;	/* indice necessario a openGL per indicare il frame */

	bool display;			/* boolean per mostrare a schermo o meno */

	bool first_frame;		/* true se mi è arrivato il primo frame */

	bool _extClose;			/* true se ho una richiesta da altri thread di chiudere la finestra */

};
#endif  /* _GLWIDGET_H */
