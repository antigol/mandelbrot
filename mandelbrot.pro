#-------------------------------------------------
#
# Project created by QtCreator 2012-06-17T22:31:27
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = mandelbrot
TEMPLATE = app


SOURCES += main.cpp\
    view.cpp \
    mandelbrot.cpp \
    palette.cpp

HEADERS  += \
    view.h \
    mandelbrot.h \
    palette.h

RESOURCES += \
    mandelbrot.qrc

OTHER_FILES += \
    mandelbrot_f.vert \
    mandelbrot_d.frag \
    mandelbrot_dd.frag \
    mandelbrot_df.frag \
    mandelbrot_f.frag \
    mandelbrot_qd.frag \
    mandelbrot_qf.frag

LIBS += -lqd

FORMS +=
