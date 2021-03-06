#version 130
#pragma optionNV(fastmath off)
#pragma optionNV(fastprecision off)

in vec2 a;

uniform float center[4];
uniform vec3 colormap[256];
uniform int accuracy;
uniform float radius;

out vec4 color;

// fonctions copiées à partir de la librairie QD
// lien : http://crd-legacy.lbl.gov/~dhbailey/mpdist/
vec2 add_df_f(in vec2 a, in float b);
vec2 add_df_df(in vec2 a, in vec2 b);
vec2 mul_df_df(in vec2 a, in vec2 b);
vec2 sqr_df(in vec2 a);

void main(void)
{
    vec2 cx = vec2(center[0], center[1]);
    vec2 cy = vec2(center[2], center[3]);

    cx = add_df_f(cx, a.x);
    cy = add_df_f(cy, a.y);

    vec2 x = cx;
    vec2 y = cy;

    int iteration = 0;

    do {
        vec2 x2 = sqr_df(x);
        vec2 y2 = sqr_df(y);

        if (x2.x + y2.x > radius)
            break;

        y = mul_df_df(x, y);
        y *= 2.0;
        y = add_df_df(y, cy);

        x = add_df_df(x2, -y2);
        x = add_df_df(x, cx);

        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = vec4(colormap[iteration & 255], 1.0);
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

vec2 add_df_f(in vec2 a, in float b)
{
    float s1, s2;
    s1 = two_sum(a.x, b, s2);
    s2 += a.y;
    s1 = quick_two_sum(s1, s2, s2);
    return vec2(s1, s2);
}

vec2 add_df_df(in vec2 a, in vec2 b)
{
    float s, e;

    s = two_sum(a.x, b.x, e);
    e += (a.y + b.y);
    s = quick_two_sum(s, e, e);
    return vec2(s, e);
}

vec2 mul_df_df(in vec2 a, in vec2 b)
{
    float p1, p2;

    p1 = two_prod(a.x, b.x, p2);
    p2 += (a.x * b.y + a.y * b.x);
    p1 = quick_two_sum(p1, p2, p2);
    return vec2(p1, p2);
}

float two_sqr(in float a, out float err) {
    float hi, lo;
    float q = a * a;
    split(a, hi, lo);
    err = ((hi * hi - q) + 2.0 * hi * lo) + lo * lo;
    return q;
}

vec2 sqr_df(in vec2 a)
{
    float p1, p2;
    float s1, s2;
    p1 = two_sqr(a.x, p2);
    p2 += 2.0 * a.x * a.y;
    p2 += a.y * a.y;
    s1 = quick_two_sum(p1, p2, s2);
    return vec2(s1, s2);
}
