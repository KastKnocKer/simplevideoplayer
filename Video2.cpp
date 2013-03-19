#include "Video2.h"

Video2::Video2(QWidget *parent) : QGLWidget(parent) {
    setMouseTracking(true);

	img = QImage();
	w = 0;
	h = 0;
	display = false;

	count = true;

}

void Video2::initializeGL() {

    /* devo sempre controllare che l'inizializzazione 
	non venga fatta prima di aver letto almeno un frame */
	/** non faccio niente fino a quando non avrò un riferimento con valore */
	if(img.isNull()){
		return;
	}

	glClearColor (0.0,0.0,0.0,1.0);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();        
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();        

	glEnable(GL_TEXTURE_2D);
	glGenTextures(3,&_texture_video);
	glBindTexture(GL_TEXTURE_2D,_texture_video);       
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);          
	glBindTexture(GL_TEXTURE_2D,_texture_video);               
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_UNSIGNED_BYTE, GL_RGBA, NULL );    

	glDisable(GL_TEXTURE_2D);
	
}

void Video2::resizeGL(int w, int h) {
    
	glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Video2::paintGL() {

	/** non faccio niente fino a quando non avrò un riferimento con valore */
	if(img.isNull()){
		return;
	};

    //dentro a questo if ci adrò una volta sola!
	if(count == true){
		this->initializeGL();
	}
	count = false;

	glClear (GL_COLOR_BUFFER_BIT);       
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();        
	//gluOrtho2D(0, win.width(),0, win.height());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();        
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,_texture_video); 
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0 , img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits() );       
	glBegin(GL_QUADS);   // in theory triangles are better
	glTexCoord2i(0,0); glVertex2i(0, img.height());
	glTexCoord2i(0,1); glVertex2i(0,0);
	glTexCoord2i(1,1); glVertex2i(img.width(),0);
	glTexCoord2i(1,0); glVertex2i(img.width(),img.height());

	//swapBuffers(); viene richiamaa in automatico alla fine del paintGL
}

void Video2::mousePressEvent(QMouseEvent *event) {

}
void Video2::mouseMoveEvent(QMouseEvent *event) {
    printf("%d, %d\n", event->x(), event->y());
}

void Video2::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}
Video2::~Video2(){

}

void Video2::setSize(int w, int h){
	this->w = w;
	this->h = h;
}

void Video2::setImg(QImage img){
	this->img = QGLWidget::convertToGLFormat(img);
}

void Video2::startdisplay(void){
	display = true;	
};