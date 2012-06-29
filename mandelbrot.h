#ifndef MANDELBROTIMAGE_H
#define MANDELBROTIMAGE_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QtOpenGL/QGLPixelBuffer>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include "qd/qd_real.h"

class Mandelbrot : public QObject
{
    Q_OBJECT
public:    
    explicit Mandelbrot(QObject *parent = 0);
    ~Mandelbrot();

    const QImage &image() const;
    void generate(int width, int height, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad = false, int sx = 1, int sy = 1);
    void generate(QSize size, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad = false, int sx = 1, int sy = 1);

    enum PaletteStyle {
        Fire,
        WaveLength,
        Rgb,
        BlackAndWite
    };

    void setPalette(enum PaletteStyle pal);

signals:
    void imageChanged();

private:
    QImage _image;
    QVector3D _colormap[256];

    void setUniformCenter(PFNGLUNIFORM1DVPROC glUniform1dv, bool quad, QGLShaderProgram &shader, qd_real cx, qd_real cy);
    QVector3D rgbFromWaveLength(double wave);
};

#endif // MANDELBROTIMAGE_H
