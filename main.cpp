#include <QtGui/QApplication>
#include "view.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    View w;
    w.resize(600, 400);
    w.show();

    return app.exec();
}
