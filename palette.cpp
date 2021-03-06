#include "palette.h"
#include <QColor>

Palette::Palette()
{
}

void Palette::add(double f, QVector3D color)
{
    _table.insert(f, color);
}

void Palette::add(double f, QColor color)
{
    _table.insert(f, QVector3D(color.redF(), color.greenF(), color.blueF()));
}

QVector3D Palette::generate(double f) const
{
    QMap<double, QVector3D>::const_iterator i = _table.constBegin();
    double lf = 0.0;
    QVector3D lc;
    while (i != _table.constEnd()) {
        if (f < i.key()) {
            double k = (f - lf) / (i.key() - lf);
            return k * i.value() + (1.0 - k) * lc;
        }
        lf = i.key();
        lc = i.value();
        ++i;
    }
    return QVector3D();
}
