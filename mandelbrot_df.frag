#version 130

in vec2 a;

uniform float center[4];
uniform vec3 colormap[256];
uniform int accuracy;
uniform float radius;

out vec4 color;

vec2 add_df_f(in vec2 a, in float b);
vec2 mul_df_f(in vec2 a, in float b);
vec2 add_df_df(in vec2 a, in vec2 b);
vec2 mul_df_df(in vec2 a, in vec2 b);
bool greater_than_df_f(in vec2 a, in float b);

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
        vec2 x2 = mul_df_df(x, x);
        vec2 y2 = mul_df_df(y, y);
        vec2 lt = add_df_df(x2, y2);
        if (greater_than_df_f(lt, radius))
            break;

        vec2 xtemp = add_df_df(x2, -y2);
        xtemp = add_df_df(xtemp, cx);

        y = mul_df_df(x, y);
        y = mul_df_f(y, 2.0);
        y = add_df_df(y, cy);

        x = xtemp;
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

vec2 mul_df_f(in vec2 a, in float b)
{
    float p1, p2;

    p1 = two_prod(a.x, b, p2);
    p2 += (a.y * b);
    p1 = quick_two_sum(p1, p2, p2);
    return vec2(p1, p2);
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

bool greater_than_df_f(in vec2 a, in float b)
{
    return (a.x > b || (a.x == b && a.y > 0.0));
}
