#version 420

in vec2 a;

uniform double center[4];
uniform vec3 colormap[256];
uniform int accuracy;

out vec4 color;

struct dd_real {
    double x[2];
};
dd_real dd_real_mk(in double c0, in double c1);
dd_real add_dd_d(in dd_real a, in double b);
dd_real mul_dd_d(in dd_real a, in double b);
dd_real add_dd_dd(in dd_real a, in dd_real b);
dd_real mul_dd_dd(in dd_real a, in dd_real b);
dd_real minus_dd(in dd_real a);
bool greater_than_dd_d(in dd_real a, in double b);

void main(void)
{
    dd_real cx = dd_real_mk(center[0], center[1]);
    dd_real cy = dd_real_mk(center[2], center[3]);

    cx = add_dd_dd(cx, dd_real_mk(double(a.x), 0.0));
    cy = add_dd_dd(cy, dd_real_mk(double(a.y), 0.0));

    dd_real x = cx;
    dd_real y = cy;

    int iteration = 0;

    do {
        dd_real x2 = mul_dd_dd(x, x);
        dd_real y2 = mul_dd_dd(y, y);
        dd_real lt = add_dd_dd(x2, y2);
        if (greater_than_dd_d(lt, 4.0))
            break;

        dd_real xtemp = add_dd_dd(x2, minus_dd(y2));
        xtemp = add_dd_dd(xtemp, cx);

        y = mul_dd_dd(x, y);
        y = mul_dd_d(y, 2.0);
        y = add_dd_dd(y, cy);

        x = xtemp;
        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = vec4(colormap[iteration & 255], 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

double two_sum(in double a, in double b, out double err)
{
    double s = a + b;
    double bb = s - a;
    err = (a - (s - bb)) + (b - bb);
    return s;
}

double quick_two_sum(in double a, in double b, out double err)
{
    double s = a + b;
    err = b - (s - a);
    return s;
}

void split(in double a, out double hi, out double lo)
{
    const double _dd_SPLITTER = 134217729.0;               // = 2^27 + 1

    double temp;
    temp = _dd_SPLITTER * a;
    hi = temp - (temp - a);
    lo = a - hi;
}

double two_prod(in double a, in double b, out double err)
{
    double a_hi, a_lo, b_hi, b_lo;
    double p = a * b;
    split(a, a_hi, a_lo);
    split(b, b_hi, b_lo);
    err = ((a_hi * b_hi - p) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;
    return p;
}

dd_real dd_real_mk(in double c0, in double c1)
{
    dd_real c;
    c.x[0] = c0;
    c.x[1] = c1;
    return c;
}

dd_real add_dd_d(in dd_real a, in double b)
{
    double s1, s2;
    s1 = two_sum(a.x[0], b, s2);
    s2 += a.x[1];
    s1 = quick_two_sum(s1, s2, s2);
    return dd_real_mk(s1, s2);
}

dd_real mul_dd_d(in dd_real a, in double b)
{
    double p1, p2;

    p1 = two_prod(a.x[0], b, p2);
    p2 += (a.x[1] * b);
    p1 = quick_two_sum(p1, p2, p2);
    return dd_real_mk(p1, p2);
}

dd_real add_dd_dd(in dd_real a, in dd_real b)
{
    double s, e;

    s = two_sum(a.x[0], b.x[0], e);
    e += (a.x[1] + b.x[1]);
    s = quick_two_sum(s, e, e);
    return dd_real_mk(s, e);
}

dd_real mul_dd_dd(in dd_real a, in dd_real b)
{
    double p1, p2;

    p1 = two_prod(a.x[0], b.x[0], p2);
    p2 += (a.x[0] * b.x[1] + a.x[1] * b.x[0]);
    p1 = quick_two_sum(p1, p2, p2);
    return dd_real_mk(p1, p2);
}

dd_real minus_dd(in dd_real a)
{
    return dd_real_mk(-a.x[0], -a.x[1]);
}

bool greater_than_dd_d(in dd_real a, in double b)
{
    return (a.x[0] > b || (a.x[0] == b && a.x[1] > 0.0));
}
