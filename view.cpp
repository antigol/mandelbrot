#include "view.h"
#include <QRectF>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QTime>
#include <iostream>

View::View(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    _scale = 1.0;
    _accuracy = 100;//_set.value("accuracy", 100).toInt();
    _cx = qd_real(_set.value("cx", "-0.743643887037158704752191506114774").toString().toLatin1().data());
    _cy = qd_real(_set.value("cy", "0.131825904205311970493132056385139").toString().toLatin1().data());
    _scale = _set.value("scale", 1.0).toFloat();

    //_cx = qd_real(0);
    //_cy = qd_real(0);
    //_scale = 1.0;

    _radius = 2.0;
    _quad = false;
    _palette = Mandelbrot::Nice;

    _timer.setSingleShot(true);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(redraw()));

//    connect(&_mandelbrot, SIGNAL(imageChanged()), this, SLOT(update()));
}

View::~View()
{
    _set.setValue("cx", QString::fromStdString(_cx.to_string(50)));
    _set.setValue("cy", QString::fromStdString(_cy.to_string(50)));
    _set.setValue("scale", _scale);
    _set.setValue("accuracy", _accuracy);
}

void View::resizeEvent(QResizeEvent *)
{
    _timer.start(200);
}

void View::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::darkGreen);
    QSizeF isize = size() * _iscale;
    QRectF targetRect(QPointF(width() - isize.width(), height() - isize.height()) / 2.0, isize);
    targetRect.moveCenter(targetRect.center() + _imove);

    painter.drawImage(targetRect, _mandelbrot.image(), QRectF(_mandelbrot.image().rect()));

    QFont f;
    f.setPointSize(7);
    painter.setFont(f);
    painter.setPen(Qt::gray);
    painter.drawText(QRectF(rect()),
                     "Keys :\n"
                     " PageUp/PageDown to zoom\n"
                     " Plus/Minus for the accuracy (maximum of iterations)\n"
                     " Return to take a screen size capture\n"
                     " 1,2,3,4 to change the colors\n"
                     " Q to switch to the quad mode (increased accuracy)\n"
                     "Mouse :\n"
                     " Drag to move\n"
                     " Wheel to zoom");
}

#include <QMouseEvent>
void View::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    _lastMousePosition = e->pos();
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);

    if (e->buttons() & Qt::LeftButton) {
        QPointF d = e->pos() - _lastMousePosition;
        _imove += d;
        d *= 2.0 * _scale / qreal(height());
        _cx -= d.x();
        _cy += d.y();

        if (_lastTime < 30)
            redraw();
        else
            update();
    }
    _lastMousePosition = e->pos();
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    redraw();
}

#include <QWheelEvent>
void View::wheelEvent(QWheelEvent *e)
{
    QWidget::wheelEvent(e);

    qreal mousey = (1.0 - qreal(e->y()) / qreal(height())) * 2.0 - 1.0; // [-1;1]
    qreal mousex = (qreal(e->x()) / qreal(width()) * 2.0 - 1.0) * qreal(width()) / qreal(height()); // [-a,a]
    qd_real x = _cx + mousex * _scale;
    qd_real y = _cy + mousey * _scale;

    GLfloat k = std::pow(0.999f, e->delta());
    _scale *= k;
    _cx = k * _cx + (1.0 - k) * x;
    _cy = k * _cy + (1.0 - k) * y;

    k = 1.0/k;
    _iscale *= k;
    _imove = k * _imove + (1.0 - k) * QPointF(e->pos() - QPoint(width(), height()) / 2);
    update();

    _timer.start(150);
    if (_lastTime < 60)
        redraw();
}

#include <QKeyEvent>
void View::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
        save();
        break;
    case Qt::Key_Plus:
        if (e->modifiers() & Qt::ControlModifier) {
            _radius *= 1.1;
            std::cout << "Radius : " << _radius << std::endl;
        } else {
            _accuracy *= 1.2;
            std::cout << "Accuracy : " << _accuracy << std::endl;
        }
        _timer.start(1000);
        break;
    case Qt::Key_Minus:
        if (e->modifiers() & Qt::ControlModifier) {
            _radius /= 1.1;
            std::cout << "Radius : " << _radius << std::endl;
        } else {
            _accuracy /= 1.2;
            if (_accuracy < 5) _accuracy = 5;
            std::cout << "Accuracy : " << _accuracy << std::endl;
        }
        _timer.start(1000);
        break;
    case Qt::Key_PageUp:
        _scale /= 1.2;
        _iscale *= 1.2;
        std::cout << "Scale : " << _scale << std::endl;
        update();
        _timer.start(1000);
        break;
    case Qt::Key_PageDown:
        _scale *= 1.2;
        _iscale /= 1.2;
        std::cout << "Scale : " << _scale << std::endl;
        update();
        _timer.start(1000);
        break;
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
        _palette = Mandelbrot::PaletteStyle(Mandelbrot::Fire + e->key() - Qt::Key_1);
        std::cout << "palette changed" << std::endl;
        _timer.start(1000);
        break;
    case Qt::Key_Q:
        _quad = !_quad;
        std::cout << "quad-floating point " << (_quad ? "en":"dis") << "able" << std::endl;
        _timer.start(1000);
        break;
    }
}

void View::save()
{
    QString file = QFileDialog::getSaveFileName(this, "Capture", _set.value("file", QDir::homePath()).toString());
    if (!file.isEmpty()) {
        _set.setValue("file", file);

        QSize ss = QApplication::desktop()->screenGeometry().size();
        std::cout << "Generate new image(" << ss.width() << "," << ss.height() << ") Cx(" << _cx << ") Cy(" << _cy << ") Scale(" << _scale << ") Accuracy(" << _accuracy << ") Radius(" << _radius << ") quad(" << (_quad ? "en":"dis") << "able)... ";
        std::cout.flush();
        _mandelbrot.generate(ss, _cx, _cy, _scale, _accuracy, _palette, _radius, _quad, 8, 8);
        _mandelbrot.image().save(file, 0, 100);
        std::cout << "Saved!" << std::endl;
    }
}

void View::redraw()
{
    std::cout.precision(25);
    std::cout << "Generate new image. Cx(" << _cx << ") Cy(" << _cy << ") Scale(" << _scale << ") Accuracy(" << _accuracy << ") Radius(" << _radius << ") quad(" << (_quad ? "en":"dis") << "able) ... ";
    std::cout << std::flush;

    QTime time;
    time.start();
    _mandelbrot.generate(size(), _cx, _cy, _scale, _accuracy, _palette, _radius, _quad, 8, 8);
    _imove.setX(0.0);
    _imove.setY(0.0);
    _iscale = 1.0;

    _lastTime = time.elapsed();
    std::cout << "Ok! " << time.elapsed() << "ms" << std::endl;

    update();
}
