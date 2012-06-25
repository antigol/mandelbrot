#include "mandelbrot.h"
#include "palette.h"

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

void Mandelbrot::generate(int width, int height, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad)
{
    generate(QSize(width, height), cx, cy, scale, accuracy, radius, quad);
}

void Mandelbrot::generate(QSize size, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad)
{
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

    shader.setUniformValueArray("colormap", _colormap, 256);
    shader.setUniformValue("aspect", GLfloat(size.width()) / GLfloat(size.height()));
    shader.setUniformValue("accuracy", GLint(accuracy));
    shader.setUniformValue("radius", GLfloat(radius*radius));
    shader.setUniformValue("scale", GLfloat(scale));

    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    shader.setAttributeArray(0, vertices, 2);

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
            float lo;
            GLfloat data[8];

            splitff(cx[0], data[0], lo);
            cx -= data[0];
            splitff(cx[0], data[1], lo);
            cx -= data[1];
            splitff(cx[0], data[2], lo);
            cx -= data[2];
            splitff(cx[0], data[3], lo);

            splitff(cy[0], data[4], lo);
            cy -= data[4];
            splitff(cy[0], data[5], lo);
            cy -= data[5];
            splitff(cy[0], data[6], lo);
            cy -= data[6];
            splitff(cy[0], data[7], lo);

            shader.setUniformValueArray("center", data, 8, 1);
        } else {
            GLfloat data[4];
            splitff(cx[0], data[0], data[1]);
            splitff(cy[0], data[2], data[3]);
            shader.setUniformValueArray("center", data, 4, 1);
        }
    }

    glViewport(0, 0, size.width(), size.height());
    glClear(GL_COLOR_BUFFER_BIT);

    shader.enableAttributeArray(0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
