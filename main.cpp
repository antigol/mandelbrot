#include <QtGui/QApplication>
#include "mandelbrot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mandelbrot w;
    w.show();
    
    return a.exec();
}
