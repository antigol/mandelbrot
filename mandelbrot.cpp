#include "mandelbrot.h"
#include <cmath>
#include <iostream>

Mandelbrot::Mandelbrot(QWidget *parent)
    : QGLWidget(parent), _shader(0)
{
    _scale = 1.0;

    _timer.setSingleShot(true);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));

    _centerx = dd_real("-0.743643887037158704752191506114774");
    _centery = dd_real("0.131825904205311970493132056385139");

    //    char str[200];
    //    _centerx.write(str, 200, 50);
    //    qDebug() << "-0.743643887037158704752191506114774";
    //    qDebug() << str;
    _render = false;
    _lowaccuracy = 60;
}

Mandelbrot::~Mandelbrot()
{
}

void Mandelbrot::initializeGL()
{
    glUniform2dv = (PFNGLUNIFORM2DVPROC) context()->getProcAddress("glUniform2dv");
    glUniform1dv = (PFNGLUNIFORM1DVPROC) context()->getProcAddress("glUniform1dv");

    if (glUniform2dv) {
        std::cout << "Yay! Hardware accelerated double precision enabled." << std::endl;
    } else {
        std::cout << "Arf! Hardware accelerated simple precision only enabled." << std::endl;
    }

    _shader = new QGLShaderProgram(this);
    _shader->addShaderFromSourceFile(QGLShader::Vertex, ":/mandelbrot.vert");
    _shader->addShaderFromSourceFile(QGLShader::Fragment, glUniform2dv ? ":/mandelbrotdd.frag" : ":/mandelbrotdf.frag");
    _shader->bindAttributeLocation("vertex", 0);
    if (!_shader->link())
        qDebug() << "link error" << _shader->log();
    _shader->bind();
    _centerLocation = _shader->uniformLocation("center");
    _scaleLocation = _shader->uniformLocation("scale");
    _ratioLocation = _shader->uniformLocation("ratio");
    _accuracyLocation = _shader->uniformLocation("accuracy");

    QVector4D colormap[256];
    for (int i = 0; i < 256; ++i) {
        colormap[i] = rgbFromWaveLength(380.0 + (i * 400.0 / 256.0));
    }
    _shader->setUniformValueArray("colormap", colormap, 256);
}

void Mandelbrot::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    _shader->setUniformValue(_ratioLocation, GLfloat(w) / GLfloat(h ? h : 1));

    if (!_render)
        _shader->setUniformValue(_accuracyLocation, _lowaccuracy);
    _render = false;

    _timer.start(1000);
}

void Mandelbrot::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    static const GLfloat vertices[] = {
        -1.0,  +1.0,
        -1.0,  -1.0,
        +1.0,  -1.0,
        +1.0,  +1.0
    };
    _shader->setAttributeArray(0, vertices, 2);

    if (glUniform2dv) {
        //        glUniform2dv(_centerLocation, 1, reinterpret_cast<const GLdouble *>(&_center));
        GLdouble data[8] = {
            _centerx.x[0], _centerx.x[1],
            _centery.x[0], _centery.x[1]
        };
        glUniform1dv(_centerLocation, 4, data);
    } else {
        //        _shader->setUniformValue(_centerLocation, _center);
        double split[4];
        qd::split(_center.x(), split[0], split[1]);
        qd::split(_center.y(), split[2], split[3]);
        GLfloat data[4] = {
            split[0], split[1], split[2], split[3]
        };
        _shader->setUniformValueArray(_centerLocation, data, 4, 1);
    }
    _shader->setUniformValue(_scaleLocation, _scale);

    _shader->enableAttributeArray(0);
    glDrawArrays(GL_QUADS, 0, 4);
    _shader->disableAttributeArray(0);
}

#include <QMouseEvent>
void Mandelbrot::mousePressEvent(QMouseEvent *e)
{
    QGLWidget::mousePressEvent(e);

    QPointF p;
    p.ry() = (1.0 - qreal(e->y()) / qreal(height())) * 2.0 - 1.0; // [-1;1]
    p.rx() = (qreal(e->x()) / qreal(width()) * 2.0 - 1.0) * qreal(width()) / qreal(height()); // [-a,a]
    p = _center + p * _scale;
    qDebug() << p;

    _mouseposition = e->posF();
}

void Mandelbrot::mouseMoveEvent(QMouseEvent *e)
{
    QGLWidget::mouseMoveEvent(e);

    if (e->buttons() & Qt::LeftButton) {
        QPointF d = e->posF() - _mouseposition;
        d.ry() = -d.y();
        d /= qreal(height());
        d *= 2.0*_scale;

        _center -= d;
        _centerx -= d.x();
        _centery -= d.y();

        _shader->setUniformValue(_accuracyLocation, _lowaccuracy);
        updateGL();
        _timer.start(500);
    }
    _mouseposition = e->posF();
}

#include <QWheelEvent>
void Mandelbrot::wheelEvent(QWheelEvent *e)
{
    QGLWidget::wheelEvent(e);

    QPointF p;
    p.ry() = (1.0 - qreal(e->y()) / qreal(height())) * 2.0 - 1.0; // [-1;1]
    p.rx() = (qreal(e->x()) / qreal(width()) * 2.0 - 1.0) * qreal(width()) / qreal(height()); // [-a,a]
    dd_real px = _centerx + p.x() * _scale;
    dd_real py = _centery + p.y() * _scale;
    p = _center + p * _scale;

    GLfloat k = std::pow(0.999f, e->delta());
    _scale *= k;
    _center = k * _center + (1.0 - k) * p;
    _centerx = k * _centerx + (1.0 - k) * px;
    _centery = k * _centery + (1.0 - k) * py;

    _shader->setUniformValue(_accuracyLocation, _lowaccuracy);
    updateGL();
    _timer.start(300);
}

void Mandelbrot::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Space:
        _shader->setUniformValue(_accuracyLocation, 100 * _lowaccuracy);
        updateGL();
        break;
    case Qt::Key_Plus:
        _scale *= 0.5;
        updateGL();
        break;
    case Qt::Key_Minus:
        _scale /= 0.5;
        updateGL();
        break;
    case Qt::Key_Return:
        _render = true;
        renderPixmap(1366, 768, true).save("mandelbrot.jpg");
        break;
    }
}

void Mandelbrot::timerTimeout()
{
    _shader->setUniformValue(_accuracyLocation, 20 * _lowaccuracy);

    updateGL();
}

QVector4D Mandelbrot::rgbFromWaveLength(double wave)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
        r = 1.0;
    }

    double s = 1.0;
    if (wave > 700.0)
        s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    else if (wave <  420.0)
        s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

    r = std::pow(r * s, 0.8);
    g = std::pow(g * s, 0.8);
    b = std::pow(b * s, 0.8);
    return QVector4D(r, g, b, 1.0);
}
