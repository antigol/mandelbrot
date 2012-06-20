#include "mandelbrotimage.h"
#include <QGLShaderProgram>

#define QUAD false

MandelbrotImage::MandelbrotImage(QObject *parent) :
    QObject(parent)
{
}

const QImage &MandelbrotImage::image() const
{
    return _image;
}

void MandelbrotImage::generate(int width, int height, qd_real cx, qd_real cy, float scale, int accuracy)
{
    QGLPixelBuffer buffer(width, height);
    buffer.makeCurrent();

    PFNGLUNIFORM1DVPROC glUniform1dv = (PFNGLUNIFORM1DVPROC) QGLContext::currentContext()->getProcAddress("glUniform1dv");

    //    if (glUniform1dv) {
    //        std::cout << "Yay! Hardware accelerated double precision enabled." << std::endl;
    //    } else {
    //        std::cout << "Arf! Hardware accelerated simple precision only enabled." << std::endl;
    //    }
//    dd_real test = dd_real(cx[0], cx[1]);
//    dd_real ta = test + scale;
//    dd_real tb = test + scale / 2.0;
//    std::cout << ta - tb << " * 2.0 = " << scale << std::endl;

    QGLShaderProgram shader;
    shader.addShaderFromSourceFile(QGLShader::Vertex, ":/mandelbrot_f.vert");
    shader.addShaderFromSourceFile(QGLShader::Fragment, glUniform1dv ? (QUAD ? ":/mandelbrot_qd.frag" : ":/mandelbrot_d.frag") : ":/mandelbrot_df.frag");
    shader.bindAttributeLocation("vertex", 0);
    if (!shader.link())
        qDebug() << "link error" << shader.log();
    shader.bind();

    QVector3D colormap[256];
    for (int i = 0; i < 256; ++i) {
        colormap[i] = fire(double(i) / 256.0);
    }
    shader.setUniformValueArray("colormap", colormap, 256);
    shader.setUniformValue("aspect", GLfloat(width) / GLfloat(height));
    shader.setUniformValue("accuracy", GLint(accuracy));
    shader.setUniformValue("scale", GLfloat(scale));

    static GLfloat const vertices[] = {
        -1, -1, 1, -1, 1, 1, -1, 1
    };
    shader.setAttributeArray(0, vertices, 2);

    if (glUniform1dv) {
        if (QUAD) {
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
        double split[4];
        qd::split(cx.x[0], split[0], split[1]);
        qd::split(cy.x[0], split[2], split[3]);
        GLfloat data[4] = {
            split[0], split[1], split[2], split[3]
        };
        shader.setUniformValueArray("center", data, 4, 1);
    }

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.enableAttributeArray(0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    shader.disableAttributeArray(0);

    _image = buffer.toImage();
    buffer.doneCurrent();
}

void MandelbrotImage::generate(QSize size, qd_real cx, qd_real cy, float scale, int accuracy)
{
    generate(size.width(), size.height(), cx, cy, scale, accuracy);
}

QVector3D MandelbrotImage::fire(double f)
{
    double k;
    if (f <= 0.17) {
        k = f / 0.17;
        return (1.0 - k) * QVector3D(0.0f, 0.0f, 0.0f) + k * QVector3D(1.0f, 0.0f, 0.0f);
    }
    if (f <= 0.83) {
        k = (f - 0.17) / (0.83 - 0.17);
        return (1.0 - k) * QVector3D(1.0f, 0.0f, 0.0f) + k * QVector3D(1.0f, 1.0f, 0.0f);
    }
    k = (f - 0.83) / (1.0 - 0.83);
    return (1.0 - k) * QVector3D(1.0f, 1.0f, 0.0f) + k * QVector3D(1.0f, 1.0f, 1.0f);
}

QVector3D MandelbrotImage::rgbFromWaveLength(double wave)
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
