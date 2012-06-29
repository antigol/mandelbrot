#include "mandelbrot.h"
#include "palette.h"
#include <QTime>
#include <cmath>
#include <GL/glext.h>

void splitff(double a, float &hi, float &lo)
{
    double temp = 536870913.0 * a; // 2^29 + 1
    hi = temp - (temp - a);
    lo = a - hi;
}

Mandelbrot::Mandelbrot(QObject *parent) :
    QObject(parent)
{
    setPalette(Fire);
}

Mandelbrot::~Mandelbrot()
{
}

const QImage &Mandelbrot::image() const
{
    return _image;
}

void Mandelbrot::generate(int width, int height, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad, int sx, int sy)
{
    generate(QSize(width, height), cx, cy, scale, accuracy, radius, quad, sx, sy);
}

void Mandelbrot::generate(QSize size, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad, int sx, int sy)
{
    if (size.width() % sx != 0)
        size.rwidth() += sx - (size.width() % sx);
    if (size.height() % sy != 0)
        size.rheight() += sy - (size.height() % sy);

    QSizeF subSize(size.width() / sx, size.height() / sy);

    QGLPixelBuffer buffer(size);
    buffer.makeCurrent();

    PFNGLUNIFORM1DVPROC glUniform1dv;
    if (QGLFormat::openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_4_0))
        glUniform1dv = (PFNGLUNIFORM1DVPROC) QGLContext::currentContext()->getProcAddress("glUniform1dv");
    else
        glUniform1dv = 0;



    QGLShaderProgram shader;
    shader.addShaderFromSourceFile(QGLShader::Vertex, ":/vert/mandelbrot_f.vert");
    if (glUniform1dv) {
        shader.addShaderFromSourceFile(QGLShader::Fragment, quad ? ":/frag/mandelbrot_qd.frag" : ":/frag/mandelbrot_dd.frag");
    } else {
        shader.addShaderFromSourceFile(QGLShader::Fragment, quad ? ":/frag/mandelbrot_qf.frag" : ":/frag/mandelbrot_df.frag");
    }

    shader.bindAttributeLocation("vertex", 0);
    if (!shader.link())
        qDebug() << "link error" << shader.log();
    shader.bind();

    GLfloat scaleY = GLfloat(scale) / GLfloat(sy);
    GLfloat aspect = subSize.width() / subSize.height();
    GLfloat scaleX = aspect * scaleY;

    shader.setUniformValueArray("colormap", _colormap, 256);
    shader.setUniformValue("aspect", aspect);
    shader.setUniformValue("accuracy", GLint(accuracy));
    shader.setUniformValue("radius", GLfloat(radius*radius));
    shader.setUniformValue("scale", scaleY);

    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    shader.setAttributeArray(0, vertices, 2);

    shader.enableAttributeArray(0);

    QTime time;
    for (int y = 0; y < sy; ++y) {
        for (int x = 0; x < sx; ++x) {
            time.start();
            int dx = 2 * x - sx + 1;
            int dy = 2 * y - sy + 1;
            setUniformCenter(glUniform1dv, quad, shader, cx + dx * scaleX, cy + dy * scaleY);

            glViewport(x * subSize.width(), y * subSize.height(), subSize.width(), subSize.height());
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            qDebug() << QPoint(x, y) << time.elapsed() << "ms";
        }
    }
    shader.disableAttributeArray(0);
    _image = buffer.toImage();
    buffer.doneCurrent();
}

void Mandelbrot::setPalette(Mandelbrot::PaletteStyle pal)
{
    Palette p;
    switch (pal) {
    case WaveLength:
        for (int i = 0; i < 256; ++i) {
            _colormap[i] = rgbFromWaveLength(380.0 + i * (780.0 - 380.0) / 256.0);
        }
        break;
    case Fire:
        p.add(0.0, QVector3D(0.0, 0.0, 0.0));
        p.add(0.17, QVector3D(1.0, 0.0, 0.0));
        p.add(0.83, QVector3D(1.0, 1.0, 0.0));
        p.add(1.0, QVector3D(1.0, 1.0, 1.0));
        for (int i = 0; i < 256; ++i) {
            _colormap[i] = p.generate(double(i) / 256.0);
        }
        break;
    case Rgb:
        p.add(0.0, QVector3D(1.0, 0.0, 0.0));
        p.add(0.333, QVector3D(0.0, 1.0, 0.0));
        p.add(0.666, QVector3D(0.0, 0.0, 1.0));
        p.add(1.0, QVector3D(1.0, 0.0, 0.0));
        for (int i = 0; i < 256; ++i) {
            _colormap[i] = p.generate(double(i) / 256.0);
        }
        break;
    case BlackAndWite:
        p.add(0.0, QVector3D(1.0, 1.0, 1.0));
        p.add(0.5, QVector3D(0.0, 0.0, 0.0));
        p.add(1.0, QVector3D(1.0, 1.0, 1.0));
        for (int i = 0; i < 256; ++i) {
            _colormap[i] = p.generate(double(i) / 256.0);
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

void Mandelbrot::setUniformCenter(PFNGLUNIFORM1DVPROC glUniform1dv, bool quad, QGLShaderProgram &shader, qd_real cx, qd_real cy)
{
    if (glUniform1dv) {
        if (quad) {
            GLdouble data[8] = {
                cx[0], cx[1], cx[2], cx[3],
                cy[0], cy[1], cy[2], cy[3]
            };
            glUniform1dv(shader.uniformLocation("center"), 8, data);
        } else {
            GLdouble data[4] = {
                cx[0], cx[1],
                cy[0], cy[1]
            };
            glUniform1dv(shader.uniformLocation("center"), 4, data);
        }
    } else {
        if (quad) {
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

            shader.setUniformValueArray("center", data, 8, 1);
        } else {
            GLfloat data[4];

            data[0] = cx[0];
            cx -= data[0];
            data[1] = cx[0];

            data[2] = cy[0];
            cy -= data[2];
            data[3] = cy[0];

            shader.setUniformValueArray("center", data, 4, 1);
        }
    }
}
