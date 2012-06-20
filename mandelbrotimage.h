#ifndef MANDELBROTIMAGE_H
#define MANDELBROTIMAGE_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QtOpenGL/QGLPixelBuffer>
#include <qd/dd_real.h>

class MandelbrotImage : public QObject
{
    Q_OBJECT
public:
    explicit MandelbrotImage(QObject *parent = 0);

    const QImage &image() const;
    void generate(int width, int height, dd_real cx, dd_real cy, float scale, int accuracy);
    void generate(QSize size, dd_real cx, dd_real cy, float scale, int accuracy);

private:
//    QGLContext *_context;

//    QPixmap *_pixmap;

    QImage _image;

    QVector3D fire(double f);
    QVector3D rgbFromWaveLength(double wave);
};

#endif // MANDELBROTIMAGE_H
