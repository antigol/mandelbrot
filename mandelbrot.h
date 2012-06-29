#ifndef MANDELBROTIMAGE_H
#define MANDELBROTIMAGE_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QtOpenGL/QGLPixelBuffer>
#include <QtOpenGL/QGLShaderProgram>
#include <QVector3D>
#include "qd/qd_real.h"
#include <GL/glext.h>

class Mandelbrot : public QObject
{
    Q_OBJECT
public:    
    explicit Mandelbrot(QObject *parent = 0);
    ~Mandelbrot();

    const QImage &image() const;
    void generate(int width, int height, const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius, bool quad = false, int sx = 1, int sy = 1);
    void generate(QSize size, const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius, bool quad = false, int sx = 1, int sy = 1);

    void initialize(QSize size, bool quad, int sx, int sy);
    void render(const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius);
    void clear();

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
    QVector3D _colormap[1024];

    ///
    PFNGLUNIFORM1DVPROC _glUniform1dv;
    QGLPixelBuffer *_buffer;
    QGLShaderProgram *_shader;
    GLfloat _aspect;
    QSizeF _subSize;
    int _sx, _sy;
    bool _quad;
    ///


    void setUniformCenter(qd_real cx, qd_real cy);
    QVector3D rgbFromWaveLength(double wave);
};

#endif // MANDELBROTIMAGE_H
