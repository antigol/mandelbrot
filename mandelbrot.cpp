#include "mandelbrot.h"
#include "palette.h"
#include <QTime>
#include <cmath>

void splitff(double a, float &hi, float &lo)
{
    double temp = 536870913.0 * a; // 2^29 + 1
    hi = temp - (temp - a);
    lo = a - hi;
}

Mandelbrot::Mandelbrot(QObject *parent) :
    QObject(parent)
{
    setPalette(WaveLength);
}

Mandelbrot::~Mandelbrot()
{
}

const QImage &Mandelbrot::image() const
{
    return _image;
}

void Mandelbrot::generate(int width, int height, const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius, bool quad, int sx, int sy)
{
    generate(QSize(width, height), cx, cy, scale, accuracy, radius, quad, sx, sy);
}

void Mandelbrot::generate(QSize size, const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius, bool quad, int sx, int sy)
{
    initialize(size, quad, sx, sy);
    render(cx, cy, scale, accuracy, radius);
    clear();
}

void Mandelbrot::initialize(QSize size, bool quad, int sx, int sy)
{
    _sx = sx;
    _sy = sy;
    _quad = quad;

    if (size.width() % sx != 0)
        size.rwidth() += sx - (size.width() % sx);
    if (size.height() % sy != 0)
        size.rheight() += sy - (size.height() % sy);

    _subSize = QSizeF(size.width() / sx, size.height() / sy);

    _buffer = new QGLPixelBuffer(size);
    _buffer->makeCurrent();

    if (QGLFormat::openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_4_0))
        _glUniform1dv = (PFNGLUNIFORM1DVPROC) QGLContext::currentContext()->getProcAddress("glUniform1dv");
    else
        _glUniform1dv = 0;



    _shader = new QGLShaderProgram();
    _shader->addShaderFromSourceFile(QGLShader::Vertex, ":/vert/mandelbrot_f.vert");
    if (_glUniform1dv) {
        _shader->addShaderFromSourceFile(QGLShader::Fragment, quad ? ":/frag/mandelbrot_qd.frag" : ":/frag/mandelbrot_dd.frag");
    } else {
        _shader->addShaderFromSourceFile(QGLShader::Fragment, quad ? ":/frag/mandelbrot_qf.frag" : ":/frag/mandelbrot_df.frag");
    }

    _shader->bindAttributeLocation("vertex", 0);
    if (!_shader->link())
        qDebug() << "link error" << _shader->log();
    _shader->bind();

    _aspect = _subSize.width() / _subSize.height();
    _shader->setUniformValueArray("colormap", _colormap, 1024);
    _shader->setUniformValue("aspect", _aspect);
}

void Mandelbrot::render(const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius)
{
    GLfloat scaleY = GLfloat(scale) / GLfloat(_sy);
    GLfloat scaleX = _aspect * scaleY;
    _shader->setUniformValue("accuracy", GLint(accuracy));
    _shader->setUniformValue("radius", GLfloat(radius*radius));
    _shader->setUniformValue("scale", scaleY);

    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    _shader->setAttributeArray(0, vertices, 2);

    _shader->enableAttributeArray(0);

    for (int y = 0; y < _sy; ++y) {
        for (int x = 0; x < _sx; ++x) {
            int dx = 2 * x - _sx + 1;
            int dy = 2 * y - _sy + 1;
            setUniformCenter(cx + dx * double(scaleX), cy + dy * double(scaleY));

            glViewport(x * _subSize.width(), y * _subSize.height(), _subSize.width(), _subSize.height());
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            _image = _buffer->toImage();
            emit imageChanged();
        }
    }

    _shader->disableAttributeArray(0);
}

void Mandelbrot::clear()
{
    delete _shader;
    delete _buffer;
}

void Mandelbrot::setPalette(Mandelbrot::PaletteStyle pal)
{
    Palette p;
    switch (pal) {
    case WaveLength:
        for (int i = 0; i < 1024; ++i) {
            _colormap[i] = rgbFromWaveLength(380.0 + i * (780.0 - 380.0) / 1024.0);
        }
        break;
    case Fire:
        p.add(0.0, QVector3D(0.0, 0.0, 0.0));
        p.add(0.17, QVector3D(1.0, 0.0, 0.0));
        p.add(0.83, QVector3D(1.0, 1.0, 0.0));
        p.add(1.0, QVector3D(1.0, 1.0, 1.0));
        for (int i = 0; i < 1024; ++i) {
            _colormap[i] = p.generate(double(i) / 1024.0);
        }
        break;
    case Rgb:
        p.add(0.0, QVector3D(1.0, 0.0, 0.0));
        p.add(0.333, QVector3D(0.0, 1.0, 0.0));
        p.add(0.666, QVector3D(0.0, 0.0, 1.0));
        p.add(1.0, QVector3D(1.0, 0.0, 0.0));
        for (int i = 0; i < 1024; ++i) {
            _colormap[i] = p.generate(double(i) / 1024.0);
        }
        break;
    case BlackAndWite:
        p.add(0.0, QVector3D(1.0, 1.0, 1.0));
        p.add(0.5, QVector3D(0.0, 0.0, 0.0));
        p.add(1.0, QVector3D(1.0, 1.0, 1.0));
        for (int i = 0; i < 1024; ++i) {
            _colormap[i] = p.generate(double(i) / 1024.0);
        }
        break;
    }
}

QVector3D Mandelbrot::rgbFromWaveLength(double wave)
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
    return QVector3D(r, g, b);
}

void Mandelbrot::setUniformCenter(qd_real cx, qd_real cy)
{
    if (_glUniform1dv) {
        if (_quad) {
            GLdouble data[8] = {
                cx[0], cx[1], cx[2], cx[3],
                cy[0], cy[1], cy[2], cy[3]
            };
            _glUniform1dv(_shader->uniformLocation("center"), 8, data);
        } else {
            GLdouble data[4] = {
                cx[0], cx[1],
                cy[0], cy[1]
            };
            _glUniform1dv(_shader->uniformLocation("center"), 4, data);
        }
    } else {
        if (_quad) {
            GLfloat data[8];

            data[0] = cx[0];
            cx -= data[0];
            data[1] = cx[0];
            cx -= data[1];
            data[2] = cx[0];
            cx -= data[2];
            data[3] = cx[0];

            data[4] = cy[0];
            cy -= data[4];
            data[5] = cy[0];
            cy -= data[5];
            data[6] = cy[0];
            cy -= data[6];
            data[7] = cy[0];

            _shader->setUniformValueArray("center", data, 8, 1);
        } else {
            GLfloat data[4];

            data[0] = cx[0];
            cx -= data[0];
            data[1] = cx[0];

            data[2] = cy[0];
            cy -= data[2];
            data[3] = cy[0];

            _shader->setUniformValueArray("center", data, 4, 1);
        }
    }
}
