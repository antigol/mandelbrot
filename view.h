#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QTimer>
#include <QSettings>
#include "mandelbrot.h"

class View : public QWidget
{
    Q_OBJECT
    
public:
    explicit View(QWidget *parent = 0);
    ~View();

protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void save();
    void redraw();

private:
    Mandelbrot _mandelbrot;

    QPointF _imove;
    qreal _iscale;

    qd_real _cx, _cy;
    float _scale;
    int _accuracy;
    float _radius;
    bool _quad;
    Mandelbrot::PaletteStyle _palette;

    QPointF _lastMousePosition;
    QTimer _timer;
    int _lastTime;

    QSettings _set;
};

#endif // VIEW_H
