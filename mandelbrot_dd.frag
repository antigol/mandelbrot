#version 420
#pragma optionNV(fastmath off)
#pragma optionNV(fastprecision off)

in vec2 a;

uniform double center[4];
uniform vec3 colormap[256];
uniform int accuracy;
uniform float radius;

out vec4 color;

dvec2 add_dd_d(in dvec2 a, in double b);
dvec2 mul_dd_d(in dvec2 a, in double b);
dvec2 add_dd_dd(in dvec2 a, in dvec2 b);
dvec2 mul_dd_dd(in dvec2 a, in dvec2 b);
bool greater_than_dd_d(in dvec2 a, in double b);

void main(void)
{
    dvec2 cx = dvec2(center[0], center[1]);
    dvec2 cy = dvec2(center[2], center[3]);

    cx = add_dd_d(cx, double(a.x));
    cy = add_dd_d(cy, double(a.y));

    dvec2 x = cx;
    dvec2 y = cy;

    int iteration = 0;

    do {
        dvec2 x2 = mul_dd_dd(x, x);
        dvec2 y2 = mul_dd_dd(y, y);
        dvec2 lt = add_dd_dd(x2, y2);
        if (greater_than_dd_d(lt, radius))
            break;

        dvec2 xtemp = add_dd_dd(x2, -y2);
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
    const double SPLITTER = 134217729.0;               // = 2^27 + 1

    double temp;
    temp = SPLITTER * a;
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

dvec2 add_dd_d(in dvec2 a, in double b)
{
    double s1, s2;
    s1 = two_sum(a.x, b, s2);
    s2 += a.y;
    s1 = quick_two_sum(s1, s2, s2);
    return dvec2(s1, s2);
}

dvec2 mul_dd_d(in dvec2 a, in double b)
{
    double p1, p2;

    p1 = two_prod(a.x, b, p2);
    p2 += (a.y * b);
    p1 = quick_two_sum(p1, p2, p2);
    return dvec2(p1, p2);
}

dvec2 add_dd_dd(in dvec2 a, in dvec2 b)
{
    double s, e;

    s = two_sum(a.x, b.x, e);
    e += (a.y + b.y);
    s = quick_two_sum(s, e, e);
    return dvec2(s, e);
}

dvec2 mul_dd_dd(in dvec2 a, in dvec2 b)
{
    double p1, p2;

    p1 = two_prod(a.x, b.x, p2);
    p2 += (a.x * b.y + a.y * b.x);
    p1 = quick_two_sum(p1, p2, p2);
    return dvec2(p1, p2);
}

bool greater_than_dd_d(in dvec2 a, in double b)
{
    return (a.x > b || (a.x == b && a.y > 0.0));
}
