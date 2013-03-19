#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

//include delle librerie per OpenGL
//#include <GL\glew.h>
//#include <GL\glfw.h>

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QtGui>
#include <QPainter>
#include <QImage>

class OpenGLWindow : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
	//COSTRUTTORE
    explicit OpenGLWindow(QGLWidget *parent = 0);
    //DISTRUTTORE
	~OpenGLWindow();

	virtual void render(QPainter *painter);
    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event);

    void exposeEvent(QExposeEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    bool m_update_pending;
    bool m_animating;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
};

#endif // OPENGLWINDOW_H