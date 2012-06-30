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
    enum PaletteStyle {
        Fire,
        WaveLength,
        Rgb,
        BlackAndWite
    };

    explicit Mandelbrot(QObject *parent = 0);
    ~Mandelbrot();

    const QImage &image() const;
    void generate(int width, int height, const qd_real &cx, const qd_real &cy, float scale, int accuracy, enum PaletteStyle palette, float radius = 2.0, bool quad = false, int sx = 1, int sy = 1);
    void generate(QSize size, const qd_real &cx, const qd_real &cy, float scale, int accuracy, enum PaletteStyle palette, float radius = 2.0, bool quad = false, int sx = 1, int sy = 1);

    void initialize(QSize size, enum PaletteStyle palette, bool quad, int sx, int sy);
    void render(const qd_real &cx, const qd_real &cy, float scale, int accuracy, float radius);
    void uninitalize();

private:
    QImage _image;

    ///
    PFNGLUNIFORM1DVPROC _glUniform1dv;
    QGLPixelBuffer *_buffer;
    QGLShaderProgram *_shader;
    GLfloat _aspect;
    QSizeF _subSize;
    int _sx, _sy;
    bool _quad;
    enum PaletteStyle _palette;
    ///


    void createColormap(QVector3D *colormap, int n, PaletteStyle pal);
    void roundUpSize(QSize &size, int sx, int sy);
    void setUniformCenter(qd_real cx, qd_real cy);
    QVector3D rgbFromWaveLength(double wave);
};

#endif // MANDELBROTIMAGE_H
