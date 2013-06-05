#include "Video.h"

Video::Video(QWidget *parent) : QGLWidget(parent) {

    setMouseTracking(true);

	pFrameRGB = NULL;
	w = 0;
	h = 0;
	display = false;

	first_frame = true;
	_extClose = false;
	setWindowTitle(tr("Finestra riproduzione"));
}

Video::~Video(){

}

void Video::initializeGL() {

    /* devo sempre controllare che l'inizializzazione 
	non venga fatta prima di aver letto almeno un frame */
	/** non faccio niente fino a quando non avrò un riferimento con valore */
	if(pFrameRGB == NULL){
		return;
	}

	// Start Of User Initialization
	glEnable(GL_TEXTURE_2D);
	glClearColor(1.0f, 0.0f, 0.0f, 0.2f);

	this->resize(w, h);
	
	glShadeModel( GL_SMOOTH );

	glGenTextures(1, &_texture_video);
	glBindTexture(GL_TEXTURE_2D, _texture_video); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
}

void Video::resizeGL(int w, int h) {
    
	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Video::paintGL() {

	/** non faccio niente fino a quando non avrò un riferimento con valore */
	if(pFrameRGB == NULL){
		return;
	};

    //dentro a questo if ci adrò una volta sola!
	if(first_frame){
		this->initializeGL();
		first_frame = false;
		return;
	}

	glBindTexture( GL_TEXTURE_2D, _texture_video );									//associo la texture corrente						
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);

	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture( GL_TEXTURE_2D, _texture_video );
	glScalef(1.0f, -1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f); 
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glScalef(1.0f, -1.0f, 1.0f);

	//swapBuffers(); viene richiamaa in automatico alla fine del paintGL
}

void Video::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Escape:
		emit Xpressed();
        break;
    default:
        event->ignore();
        break;
    }
}

void Video::setSize(int w, int h){
	this->w = w;
	this->h = h;
}

void Video::setFrame(AVFrame* pFrameRGB){
	this->pFrameRGB = pFrameRGB;
}

void Video::startdisplay(void){
	display = true;	
};

/**
		Imposta titolo finestra
		@param title titolo finestra
	*/
void Video::setTitle(QString title){
	setWindowTitle(title);
}



////////////////////////////////////////////////////////////////////////////////////

/**
SLOT per imporre la chiusura della finestra
*/
void Video::closeWindow(){
	_extClose = true;
	this->close();
}

/**
ridefinizione dell'avvento di chiusura della finestra, emetto uno specifico segnale
*/
void Video::closeEvent(QCloseEvent *event){
	
	//CASO PREMO CHIUDI FINESTRA
	if(!_extClose){
		
		//qDebug() << "VIDEO - WINDOW CLOSING INT";
		
		emit Xpressed();
		event->ignore();
	}
	else{
		
		//qDebug() << "VIDEO - WINDOW CLOSING ext";
		
		emit windowClosing();
	}
	_extClose = false;
}
