#ifndef MANDELBROTIMAGE_H
#define MANDELBROTIMAGE_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QtOpenGL/QGLPixelBuffer>
#include <QVector3D>
#include <qd/qd_real.h>

class Mandelbrot : public QObject
{
    Q_OBJECT
public:    
    explicit Mandelbrot(QObject *parent = 0);

    const QImage &image() const;
    void generate(int width, int height, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad = false);
    void generate(QSize size, qd_real cx, qd_real cy, float scale, int accuracy, float radius, bool quad = false);

    enum PaletteStyle {
        Fire,
        WaveLength,
        Rgb,
        BlackAndWite
    };

    void setPalette(enum PaletteStyle pal);

private:
    QImage _image;
    QVector3D _colormap[256];

    QVector3D rgbFromWaveLength(double wave);
};

#endif // MANDELBROTIMAGE_H
