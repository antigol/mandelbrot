#-------------------------------------------------
#
# Project created by QtCreator 2012-06-17T22:31:27
#
#-------------------------------------------------

QT       += core gui opengl widgets

TARGET = mandelbrot
TEMPLATE = app


SOURCES += main.cpp\
    view.cpp \
    mandelbrot.cpp \
    palette.cpp \
    qd/qd_real.cpp \
    qd/util.cpp \
    qd/qd_const.cpp \
    qd/fpu.cpp \
    qd/dd_real.cpp \
    qd/dd_const.cpp \
    qd/c_qd.cpp \
    qd/c_dd.cpp \
    qd/bits.cpp

HEADERS  += \
    view.h \
    mandelbrot.h \
    palette.h \
    qd/util.h \
    qd/qd_real.h \
    qd/qd_inline.h \
    qd/qd_config.h \
    qd/inline.h \
    qd/fpu.h \
    qd/dd_real.h \
    qd/dd_inline.h \
    qd/c_qd.h \
    qd/config.h \
    qd/c_dd.h \
    qd/bits.h

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
