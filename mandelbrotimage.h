#ifndef MANDELBROTIMAGE_H
#define MANDELBROTIMAGE_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QtOpenGL/QGLPixelBuffer>
#include <qd/qd_real.h>

class MandelbrotImage : public QObject
{
    Q_OBJECT
public:
    explicit MandelbrotImage(QObject *parent = 0);

    const QImage &image() const;
    void generate(int width, int height, qd_real cx, qd_real cy, float scale, int accuracy);
    void generate(QSize size, qd_real cx, qd_real cy, float scale, int accuracy);

private:
    QImage _image;

    QVector3D fire(double f);
    QVector3D rgbFromWaveLength(double wave);
};

#endif // MANDELBROTIMAGE_H
