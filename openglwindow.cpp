#include "openglwindow.h"


/* classe opengl
   renzi prova a inizializzare qui la tua finestra dovrebbe comparire automaticamente
   nella finestra del player
*/

//COSTRUTTORE
/**
The window's surface type must be set to QSurface::OpenGLSurface
to indicate that the window is to be used for OpenGL rendering
and not for rendering raster content with QPainter
*/
OpenGLWindow::OpenGLWindow(QGLWidget *parent)
    : QGLWidget(parent)
    , m_update_pending(false)
    , m_animating(false)
    , m_context(0)
    , m_device(0)
{
    setSurfaceType(QGLWidget::OpenGLSurface);
}




/**
Any OpenGL initialization needed can be done by overriding the initialize() function,
which is called once before the first call to render(), with a valid current QOpenGLContext
*/
void OpenGLWindow::initialize()
{
	//Start of User Initialization
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.4f, 0.4f, 0.0f);


}

/**
the default render(QPainter *) and initialize() implementations are empty,
whereas the default render() implementation initializes a QOpenGLPaintDevice
and then calls into render(QPainter *)
*/
void OpenGLWindow::render()
{
    if (!m_device)
        m_device = new QOpenGLPaintDevice;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_device->setSize(size());

    QPainter painter(m_device);
    render(&painter);
}

void OpenGLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

/**
The renderLater() function simply puts an update request event on the event loop,
which leads to renderNow() being called once the event gets processed.
*/
void OpenGLWindow::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QGLWidget::event(event);
    }
}

/**
The exposeEvent() is the notification to the window that its exposure, meaning visibility, on the screen has changed.
When the expose event is received you can query QWindow::isExposed() to find out whether or not the window is currently exposed.
Do not render to or call QOpenGLContext::swapBuffers() on a window before it has received its first expose event,
as before then its final size might be unknown, and in addition what is rendered might not even end up on the screen.
*/
void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void OpenGLWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

/**
In renderNow() we return if we are not currently exposed, in which case rendering is delayed until we actually get an expose event.
If we have not yet done so, we create the QOpenGLContext with the same QSurfaceFormat as was set on the OpenGLWindow,
and call initialize() for the sake of the sub class, and initializeOpenGLFunctions() in order
for the QOpenGLFunctions super class to be associated with the correct QOpenGLContext.
In any case we make the context current by calling QOpenGLContext::makeCurrent(), call render() to do the actual rendering,
and finally we schedule for the rendered contents to be made visible by calling QOpenGLContext::swapBuffers() with the OpenGLWindow as parameter.
Once the rendering of a frame using an OpenGL context is initiated by calling QOpenGLContext::makeCurrent(),
giving the surface on which to render as a parameter, OpenGL commands can be issued. 
The commands can be issued either directly by including <qopengl.h>, which also includes the system's OpenGL headers,
or as by using QOpenGLFunctions, which can either be inherited from for convenience, or accessed using QOpenGLContext::functions().
QOpenGLFunctions gives access to all the OpenGL ES 2.0 level OpenGL calls that are not already standard in both OpenGL ES 2.0 and desktop OpenGL.
*/
void OpenGLWindow::renderNow()
{
    if (!isExposed())
        return;

    m_update_pending = false;

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

/*
void GLWidget::initializeGL(){

	//Start of User Initialization
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.4f, 0.4f, 0.0f);

	// Start Of User Initialization
	//float angle		= 0.0f;										// Set Starting Angle To Zero
	//hdd = DrawDibOpen();										// Grab A Device Context For Our Dib
	//glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
	//glClearDepth (1.0f);										// Depth Buffer Setup
	//glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	//glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	//glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	//glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	//quadratic = gluNewQuadric();								// Create A Pointer To The Quadric Object
	//gluQuadricNormals(quadratic, GLU_SMOOTH);					// Create Smooth Normals 
	//gluQuadricTexture(quadratic, GL_TRUE);						// Create Texture Coords 

	//glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	// Set Texture Max Filter
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);	// Set Texture Min Filter

	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping


	//Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        //return false;
    }

    //return true;

}
*/


//void GLWidget::resizeGL(int w,int h){
//	glViewport(0,0, w, h);
//}

/**
The paintGL() function is used to paint the contents of the scene onto the widget
*/
//void GLWidget::paintGL(){
//
//	prova = QImage("prova", "jpg");												//carico immaigne di prova
//	QRect rect(0, 0, prova.width(), prova.height());							//define the area used to display
//
//	QPainter painter(this);														//istanzio il painter che andra a disegnare sulla finestra
//	painter.begin(this);
//	//painter.drawImage(rect, prova);
//	//prova->draw();
//	//painter.setRenderHint(QPainter::Antialiasing);
//	//painter.drawText(QRect(-50, -50, 100, 100), Qt::AlignCenter, QStringLiteral("Qt"));
//	painter.end();
//
//
//}

/*
void GLWidget::drawImage(){

	makeCurrent();

	glLoadIdentity();

	QImage image; // the image rendered

	if (!prova.isNull()){
         
        glPushMatrix();
		{
			int imW = prova.width();
            int imH = prova.height();
             
            // The image is to be resized to fit the widget?
            if( imW != this-&gt;size().width() &amp;&amp;
                    imH != this-&gt;size().height() )
            {
                 
                image = prova.scaled( //this-&gt;size(),
                                             QSize(mOutW,mOutH),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation
                                             );
                 
                //qDebug( QString( "Image size: (%1x%2)").arg(imW).arg(imH).toAscii() );
            }
            else
                image = prova;
             
            // --->Centering image in draw area
             
            glRasterPos2i( mPosX, mPosY );
            // < --- Centering image in draw area
             
            imW = image.width();
            imH = image.height();
             
            glDrawPixels( imW, imH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
		}
        glPopMatrix();
         
        // end
        glFlush();
    }
}*/

/*
void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    helper->paint(&painter, event, elapsed);
    painter.end();
}*/

/*
int LoadTexture(string file)
        {
            Bitmap bitmap = new Bitmap(file);

            int tex;
            GL.Hint(HintTarget.PerspectiveCorrectionHint, HintMode.Nicest);

            GL.GenTextures(1, out tex);
            GL.BindTexture(TextureTarget.Texture2D, tex);

            BitmapData data = bitmap.LockBits(new System.Drawing.Rectangle(0, 0, bitmap.Width, bitmap.Height),
                ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba, data.Width, data.Height, 0,
                OpenTK.Graphics.OpenGL.PixelFormat.Bgra, PixelType.UnsignedByte, data.Scan0);
            bitmap.UnlockBits(data);


            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Linear);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.Repeat);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.Repeat);

            return tex;
}*/

