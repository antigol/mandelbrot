#include <QApplication>
#include "view.h"

#include <QTime>
#include <cmath>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    View w;
    w.resize(600, 400);
    w.show();

    return app.exec();

    Mandelbrot m; //11004 5.58954e-15 26095 ms

    //QSize size(256, 256);
    QSize size(3000, 3000);
    qd_real cx = qd_real("-0.743643887037158704752191506114774");
    qd_real cy = qd_real("0.131825904205311970493132056385139");

    float scale = 1.0;
    int counter = 1;

    m.initialize(size, Mandelbrot::Gold, false, 8, 8);

    QTime t;
    t.start();
    //while (scale > 1e-30) {
        int accurancy = 256 + 100 * std::pow(std::abs(std::log(scale)), 1.34);
        m.render(cx, cy, scale, 1024, 2.0);
        m.image().save(QString("img_%1.jpg").arg(counter), 0, 90);
        qDebug() << accurancy << scale << t.restart() << "ms";

        scale /= 1.2;
        counter++;
    //}

    m.uninitalize();
    return 0;
}
