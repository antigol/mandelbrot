#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLShaderProgram>
#include <QtGui/QVector4D>
#include <QtCore/QTimer>

class Mandelbrot : public QGLWidget
{
    Q_OBJECT
    
public:
    Mandelbrot(QWidget *parent = 0);
    ~Mandelbrot();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private slots:
    void timerTimeout();

private:
    QVector4D rgbFromWaveLength(double wave);
    QGLShaderProgram *_shader;

    int _centerLocation;
    int _scaleLocation;
    int _ratioLocation;
    int _accuracyLocation;

    QPointF _center;
    GLfloat _scale;

    QPointF _mouseposition;

    QTimer _timer;

    PFNGLUNIFORM2DVPROC glUniform2dv;
};

#endif // MANDELBROT_H
