#include <QtGui/QApplication>
#include "mandelbrot.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Mandelbrot w;
    w.show();
    
    return app.exec();
}
