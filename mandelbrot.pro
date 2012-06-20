#-------------------------------------------------
#
# Project created by QtCreator 2012-06-17T22:31:27
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = mandelbrot
TEMPLATE = app


SOURCES += main.cpp\
    mandelbrotimage.cpp \
    view.cpp

HEADERS  += \
    mandelbrotimage.h \
    view.h

RESOURCES += \
    mandelbrot.qrc

OTHER_FILES += \
    mandelbrotd.frag \
    mandelbrotqd.frag \
    mandelbrotdf.frag \
    mandelbrot.vert \
    mandelbrotf.frag \
    mandelbrotdd.frag

LIBS += -lqd

FORMS +=
