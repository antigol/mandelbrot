#include <QtGui/QApplication>
#include "mandelbrot.h"

#include <cmath>
#include <QDebug>
#include <qd/qd_real.h>

struct FPN
{
    uint n3, n2, n1, n0;
};

void fpnadd2positive(FPN a, FPN b, FPN &c);
void fpnsub2positiveAb(FPN a, FPN b, FPN &c);
int fpncomp2positive(FPN a, FPN b);
int fpncomp(FPN a, FPN b);
void fpnsub(FPN a, FPN b, FPN &c);
void fpnadd(FPN a, FPN b, FPN &c);
void fpninv(FPN &a);
bool fpnisnull(FPN a);
bool fpnispos(FPN a);
FPN fpnfrom(float a);
float fpntofloat(FPN a);

int main(int argc, char *argv[])
{
    FPN a = fpnfrom(-1.3e-20);
    FPN b = fpnfrom(12.55);

    FPN c;
    fpnadd(a, b, c);
    qDebug() << c.n3 << c.n2 << c.n1 << c.n0;

    FPN d;
    fpnsub(c, b, d);
    qDebug() << fpntofloat(d);

    return 0;

    QApplication app(argc, argv);
    Mandelbrot w;
    w.show();
    
    return app.exec();
}

void fpnadd2positive(FPN a, FPN b, FPN &c)
{
    // additionne exclusivement deux nombres positifs entre eux
    // il faut regarder s'il y a eu dépassement (seul les 31 permier bits sont utilisée)
    c.n0 = a.n0 + b.n0;
    if (c.n0 & 0x80000000) {
        c.n0 -= 0x80000000;
        a.n1 += 1;
    }

    c.n1 = a.n1 + b.n1;
    if (c.n1 & 0x80000000) {
        c.n1 -= 0x80000000;
        a.n2 += 1;
    }

    c.n2 = a.n2 + b.n2;
    if (c.n2 & 0x80000000) {
        c.n2 -= 0x80000000;
        a.n3 += 1;
    }

    c.n3 = a.n3 + (b.n3 - 0x80000000);
}

void fpnsub2positiveAb(FPN a, FPN b, FPN &c)
{
    // soustrait deux nombre (A>=B), le resultat peu être negatif
    c.n0 = a.n0 - b.n0;
    if (c.n0 & 0x80000000) {
        c.n0 += 0x80000000;
        a.n1 -= 1;
    }

    c.n1 = a.n1 - b.n1;
    if (c.n1 & 0x80000000) {
        c.n1 += 0x80000000;
        a.n2 -= 1;
    }

    c.n2 = a.n2 - b.n2;
    if (c.n2 & 0x80000000) {
        c.n2 += 0x80000000;
        a.n3 -= 1;
    }

    c.n3 = a.n3 - (b.n3 - 0x80000000);
}

void fpnsub(FPN a, FPN b, FPN &c)
{
    if (!fpnispos(a) && !fpnispos(b)) {
        fpninv(a);
        fpninv(b);
        fpnsub(b, a, c);
    } else if (!fpnispos(b)) {
        fpninv(b);
        fpnadd2positive(a, b, c);
    } else if (!fpnispos(a)) {
        fpninv(a);
        fpnadd2positive(a, b, c);
        fpninv(c);
    } else {
        int compare = fpncomp(a, b);
        if (compare >= 0) {
            fpnsub2positiveAb(a, b, c);
        } else {
            fpnsub2positiveAb(b, a, c);
            fpninv(c);
        }
    }
}

void fpnadd(FPN a, FPN b, FPN &c)
{
    fpninv(b);
    fpnsub(a, b, c);
}

void fpninv(FPN &a)
{
    a.n3 ^= 0x80000000;
}

int fpncomp(FPN a, FPN b)
{
    if (fpnispos(a) && fpnispos(b)) {
        return fpncomp2positive(a, b);
    }
    if (!fpnispos(a) && !fpnispos(b)) {
        fpninv(a);
        fpninv(b);
        return fpncomp2positive(b, a);
    }
    if (fpnispos(a)) {
        if (fpnisnull(a) && fpnisnull(b))
            return 0;
        else
            return 1;
    }
    if (fpnisnull(a) && fpnisnull(b))
        return 0;
    else
        return -1;
}

int fpncomp2positive(FPN a, FPN b)
{
    int c = -1;
    if (a.n3 > b.n3)
        c = 1;
    else if (a.n3 == b.n3) {
        if (a.n2 > b.n2)
            c = 1;
        else if (a.n2 == b.n2) {
            if (a.n1 > b.n1)
                c = 1;
            else if (a.n1 == b.n1) {
                if (a.n0 > b.n0)
                    c = 1;
                else if (a.n0 == b.n0) {
                    c = 0;
                }
            }
        }
    }

    return c;
}

bool fpnisnull(FPN a)
{
    return (a.n3 & 0x7FFFFFFF) == 0 && a.n2 == 0 && a.n1 == 0 && a.n0 == 0;
}

bool fpnispos(FPN a)
{
    return a.n3 & 0x80000000;
}

FPN fpnfrom(float a)
{
    FPN c;

    bool positive = a > 0.0;
    a = (a < 0) ? -a : a;

    c.n3 = int(std::floor(a));
    a -= float(c.n3);
    if (positive)
        c.n3 += 0x80000000;

    a *= float(0x80000000);
    c.n2 = int(std::floor(a));
    a -= float(c.n2);

    a *= float(0x80000000);
    c.n1 = int(std::floor(a));
    a -= float(c.n1);

    a *= float(0x80000000);
    c.n0 = int(std::floor(a));

    return c;
}

float fpntofloat(FPN a)
{
    bool positive = false;
    if (fpnispos(a)) {
        positive = true;
        a.n3 -= 0x80000000;
    }

    float f = a.n3;
    f += float(a.n2) / float(0x80000000);
    f += float(a.n1) / float(0x80000000) / float(0x80000000);
    f += float(a.n0) / float(0x80000000) / float(0x80000000) / float(0x80000000);

    if (!positive)
        f = -f;

    return f;
}
