#include "view.h"
#include <QRectF>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>

View::View(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    _cx = dd_real("-0.743643887037158704752191506114774");
    _cy = dd_real("0.131825904205311970493132056385139");
    _scale = 1.0;
    _accuracy = 300;

    _timer.setSingleShot(true);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateMandelbrotAndDraw()));

    updateMandelbrot();
}

View::~View()
{
}

void View::resizeEvent(QResizeEvent *)
{
    updateMandelbrot();
}

void View::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::darkGreen);
    QSizeF isize = size() * _iscale;
    QRectF targetRect(QPointF(width() - isize.width(), height() - isize.height()) / 2.0, isize);
    targetRect.moveCenter(targetRect.center() + _imove);

    painter.drawImage(targetRect, _mandelbrot.image(), QRectF(_mandelbrot.image().rect()));
}

#include <QMouseEvent>
void View::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    _lastMousePosition = e->posF();
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);

    if (e->buttons() & Qt::LeftButton) {
        QPointF d = e->posF() - _lastMousePosition;
        _imove += d;
        d *= 2.0 * _scale / qreal(height());

        _cx -= d.x();
        _cy += d.y();
        update();
    }
    _lastMousePosition = e->posF();
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    updateMandelbrot();
    update();
}

#include <QWheelEvent>
void View::wheelEvent(QWheelEvent *e)
{
    QWidget::wheelEvent(e);

    qreal mousey = (1.0 - qreal(e->y()) / qreal(height())) * 2.0 - 1.0; // [-1;1]
    qreal mousex = (qreal(e->x()) / qreal(width()) * 2.0 - 1.0) * qreal(width()) / qreal(height()); // [-a,a]
    dd_real x = _cx + mousex * _scale;
    dd_real y = _cy + mousey * _scale;

    GLfloat k = std::pow(0.999f, e->delta());
    _scale *= k;
    _cx = k * _cx + (1.0 - k) * x;
    _cy = k * _cy + (1.0 - k) * y;

    k = 1.0/k;
    _iscale *= k;
    _imove = k * _imove + (1.0 - k) * QPointF(e->pos() - QPoint(width(), height()) / 2);
    update();

    _timer.start(100);
}

#include <QKeyEvent>
void View::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
        int accuracy = QInputDialog::getInt(this, "Accuracy", "2000 is a good value", 2000, 1);
        QString file = QFileDialog::getSaveFileName(this, "Capture", _set.value("file", QDir::homePath()).toString());
        if (!file.isEmpty()) {
            _set.setValue("file", file);
            _mandelbrot.generate(QApplication::desktop()->screenGeometry().size(),
                                 _cx, _cy, _scale, accuracy);
            _mandelbrot.image().save(file, 0, 100);
        }
        break;
    }
}

void View::updateMandelbrot()
{
    _mandelbrot.generate(size(), _cx, _cy, _scale, _accuracy);
    _imove.setX(0.0);
    _imove.setY(0.0);
    _iscale = 1.0;
}

void View::updateMandelbrotAndDraw()
{
    updateMandelbrot();
    update();
}
