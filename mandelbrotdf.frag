#version 130

in vec2 a;

uniform float center[4];
uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

struct df_real {
    float x[4];
};
df_real df_real_mk(in float c0, in float c1);
df_real add_df_f(in df_real a, in float b);
df_real mul_df_f(in df_real a, in float b);
df_real add_df_df(in df_real a, in df_real b);
df_real mul_df_df(in df_real a, in df_real b);
df_real minus_df(in df_real a);
bool greater_than_df_f(in df_real a, in float b);

void main(void)
{
    df_real cx = df_real_mk(center[0], center[1]);
    df_real cy = df_real_mk(center[2], center[3]);

    cx = add_df_f(cx, a.x);
    cy = add_df_f(cy, a.y);

    df_real x = cx;
    df_real y = cy;

    int iteration = 0;

    do {
        df_real x2 = mul_df_df(x, x);
        df_real y2 = mul_df_df(y, y);
        df_real lt = add_df_df(x2, y2);
        if (greater_than_df_f(lt, 4.0))
            break;

        df_real xtemp = add_df_df(x2, minus_df(y2));
        xtemp = add_df_df(xtemp, cx);

        y = mul_df_df(x, y);
        y = mul_df_f(y, 2.0);
        y = add_df_df(y, cy);

        x = xtemp;
        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = colormap[iteration & 255];
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

float two_sum(in float a, in float b, out float err)
{
    float s = a + b;
    float bb = s - a;
    err = (a - (s - bb)) + (b - bb);
    return s;
}

float quick_two_sum(in float a, in float b, out float err)
{
    float s = a + b;
    err = b - (s - a);
    return s;
}

void split(in float a, out float hi, out float lo)
{
    const float _df_SPLITTER = 4097.0;               // = 2^12 + 1

    float temp;
    temp = _df_SPLITTER * a;
    hi = temp - (temp - a);
    lo = a - hi;
}

float two_prod(in float a, in float b, out float err)
{
    float a_hi, a_lo, b_hi, b_lo;
    float p = a * b;
    split(a, a_hi, a_lo);
    split(b, b_hi, b_lo);
    err = ((a_hi * b_hi - p) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;
    return p;
}

df_real df_real_mk(in float c0, in float c1)
{
    df_real c;
    c.x[0] = c0;
    c.x[1] = c1;
    return c;
}

df_real add_df_f(in df_real a, in float b)
{
    float s1, s2;
    s1 = two_sum(a.x[0], b, s2);
    s2 += a.x[1];
    s1 = quick_two_sum(s1, s2, s2);
    return df_real_mk(s1, s2);
}

df_real mul_df_f(in df_real a, in float b)
{
    float p1, p2;

    p1 = two_prod(a.x[0], b, p2);
    p2 += (a.x[1] * b);
    p1 = quick_two_sum(p1, p2, p2);
    return df_real_mk(p1, p2);
}

df_real add_df_df(in df_real a, in df_real b)
{
    float s, e;

    s = two_sum(a.x[0], b.x[0], e);
    e += (a.x[1] + b.x[1]);
    s = quick_two_sum(s, e, e);
    return df_real_mk(s, e);
}

df_real mul_df_df(in df_real a, in df_real b)
{
    float p1, p2;

    p1 = two_prod(a.x[0], b.x[0], p2);
    p2 += (a.x[0] * b.x[1] + a.x[1] * b.x[0]);
    p1 = quick_two_sum(p1, p2, p2);
    return df_real_mk(p1, p2);
}

df_real minus_df(in df_real a)
{
    return df_real_mk(-a.x[0], -a.x[1]);
}

bool greater_than_df_f(in df_real a, in float b)
{
    return (a.x[0] > b || (a.x[0] == b && a.x[1] > 0.0));
}
