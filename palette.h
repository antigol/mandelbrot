#ifndef PALETTE_H
#define PALETTE_H

#include <QVector3D>
#include <QMap>

class Palette
{
public:
    Palette();
    void add(double f, QVector3D color);
    void add(double f, QColor color);
    QVector3D generate(double f) const;

private:
    QMap<double, QVector3D> _table;
};

#endif // PALETTE_H
