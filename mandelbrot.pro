#-------------------------------------------------
#
# Project created by QtCreator 2012-06-17T22:31:27
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = mandelbrot
TEMPLATE = app


SOURCES += main.cpp\
        mandelbrot.cpp

HEADERS  += mandelbrot.h

RESOURCES += \
    mandelbrot.qrc

OTHER_FILES += \
    mandelbrot.vert \
    mandelbrotd.frag \
    mandelbrot.frag
