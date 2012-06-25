#version 420
#pragma optionNV(fastmath off)
#pragma optionNV(fastprecision off)

in vec2 a;

uniform double center[8];
uniform vec3 colormap[256];
uniform int accuracy;
uniform float radius;

out vec4 color;

dvec4 add_qd_d(in dvec4 a, in double b);
dvec4 add_qd_qd(in dvec4 a, in dvec4 b);
dvec4 mul_qd_qd(in dvec4 a, in dvec4 b);
bool greater_than_qd_d(in dvec4 a, in double b);

void main(void)
{
    dvec4 cx = dvec4(center[0], center[1], center[2], center[3]);
    dvec4 cy = dvec4(center[4], center[5], center[6], center[7]);

    cx = add_qd_d(cx, double(a.x));
    cy = add_qd_d(cy, double(a.y));

    dvec4 x = cx;
    dvec4 y = cy;

    int iteration = 0;

    do {
        dvec4 x2 = mul_qd_qd(x, x);
        dvec4 y2 = mul_qd_qd(y, y);
        dvec4 lt = add_qd_qd(x2, y2);
        if (greater_than_qd_d(lt, radius))
            break;

        dvec4 xtemp = add_qd_qd(x2, -y2);
        xtemp = add_qd_qd(xtemp, cx);

        y = mul_qd_qd(x, y);
        y *= 2.0;
        y = add_qd_qd(y, cy);

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

void renorm(inout double c0, inout double c1, inout double c2, inout double c3, inout double c4)
{
    double s0, s1, s2 = 0.0, s3 = 0.0;

    //    if (QD_ISINF(c0)) return;

    s0 = quick_two_sum(c3, c4, c4);
    s0 = quick_two_sum(c2, s0, c3);
    s0 = quick_two_sum(c1, s0, c2);
    c0 = quick_two_sum(c0, s0, c1);

    s0 = c0;
    s1 = c1;

    s0 = quick_two_sum(c0, c1, s1);
    if (s1 != 0.0) {
        s1 = quick_two_sum(s1, c2, s2);
        if (s2 != 0.0) {
            s2 = quick_two_sum(s2, c3, s3);
            if (s3 != 0.0)
                s3 += c4;
            else
                s2 += c4;
        } else {
            s1 = quick_two_sum(s1, c3, s2);
            if (s2 != 0.0)
                s2 = quick_two_sum(s2, c4, s3);
            else
                s1 = quick_two_sum(s1, c4, s2);
        }
    } else {
        s0 = quick_two_sum(s0, c2, s1);
        if (s1 != 0.0) {
            s1 = quick_two_sum(s1, c3, s2);
            if (s2 != 0.0)
                s2 = quick_two_sum(s2, c4, s3);
            else
                s1 = quick_two_sum(s1, c4, s2);
        } else {
            s0 = quick_two_sum(s0, c3, s1);
            if (s1 != 0.0)
                s1 = quick_two_sum(s1, c4, s2);
            else
                s0 = quick_two_sum(s0, c4, s1);
        }
    }

    c0 = s0;
    c1 = s1;
    c2 = s2;
    c3 = s3;
}

dvec4 add_qd_d(in dvec4 a, in double b)
{
    double c0, c1, c2, c3;
    double e;

    c0 = two_sum(a.x, b, e);
    c1 = two_sum(a.y, e, e);
    c2 = two_sum(a.z, e, e);
    c3 = two_sum(a.w, e, e);

    renorm(c0, c1, c2, c3, e);

    return dvec4(c0, c1, c2, c3);
}

void split(in double a, out double hi, out double lo)
{
    const double _QD_SPLITTER = 134217729.0;               // = 2^27 + 1

    double temp;
    temp = _QD_SPLITTER * a;
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

void three_sum(inout double a, inout double b, inout double c)
{
    double t1, t2, t3;
    t1 = two_sum(a, b, t2);
    a  = two_sum(c, t1, t3);
    b  = two_sum(t2, t3, c);
}

void three_sum2(inout double a, inout double b, inout double c)
{
    double t1, t2, t3;
    t1 = two_sum(a, b, t2);
    a  = two_sum(c, t1, t3);
    b = t2 + t3;
}

dvec4 add_qd_qd(in dvec4 a, in dvec4 b)
{
    double s0, s1, s2, s3;
    double t0, t1, t2, t3;

    double v0, v1, v2, v3;
    double u0, u1, u2, u3;
    double w0, w1, w2, w3;

    s0 = a.x + b.x;
    s1 = a.y + b.y;
    s2 = a.z + b.z;
    s3 = a.w + b.w;

    v0 = s0 - a.x;
    v1 = s1 - a.y;
    v2 = s2 - a.z;
    v3 = s3 - a.w;

    u0 = s0 - v0;
    u1 = s1 - v1;
    u2 = s2 - v2;
    u3 = s3 - v3;

    w0 = a.x - u0;
    w1 = a.y - u1;
    w2 = a.z - u2;
    w3 = a.w - u3;

    u0 = b.x - v0;
    u1 = b.y - v1;
    u2 = b.z - v2;
    u3 = b.w - v3;

    t0 = w0 + u0;
    t1 = w1 + u1;
    t2 = w2 + u2;
    t3 = w3 + u3;

    s1 = two_sum(s1, t0, t0);
    three_sum(s2, t0, t1);
    three_sum2(s3, t0, t2);
    t0 = t0 + t1 + t3;

    /* renormalize */
    renorm(s0, s1, s2, s3, t0);
    return dvec4(s0, s1, s2, s3);
}

dvec4 mul_qd_qd(in dvec4 a, in dvec4 b)
{
    double p0, p1, p2, p3, p4, p5;
    double q0, q1, q2, q3, q4, q5;
    double p6, p7, p8, p9;
    double q6, q7, q8, q9;
    double r0, r1;
    double t0, t1;
    double s0, s1, s2;

    p0 = two_prod(a.x, b.x, q0);

    p1 = two_prod(a.x, b.y, q1);
    p2 = two_prod(a.y, b.x, q2);

    p3 = two_prod(a.x, b.z, q3);
    p4 = two_prod(a.y, b.y, q4);
    p5 = two_prod(a.z, b.x, q5);

    /* Start Accumulation */
    three_sum(p1, p2, q0);

    /* Six-Three Sum  of p2, q1, q2, p3, p4, p5. */
    three_sum(p2, q1, q2);
    three_sum(p3, p4, p5);
    /* compute (s0, s1, s2) = (p2, q1, q2) + (p3, p4, p5). */
    s0 = two_sum(p2, p3, t0);
    s1 = two_sum(q1, p4, t1);
    s2 = q2 + p5;
    s1 = two_sum(s1, t0, t0);
    s2 += (t0 + t1);

    /* O(eps^3) order terms */
    p6 = two_prod(a.x, b.w, q6);
    p7 = two_prod(a.y, b.z, q7);
    p8 = two_prod(a.z, b.y, q8);
    p9 = two_prod(a.w, b.x, q9);

    /* Nine-Two-Sum of q0, s1, q3, q4, q5, p6, p7, p8, p9. */
    q0 = two_sum(q0, q3, q3);
    q4 = two_sum(q4, q5, q5);
    p6 = two_sum(p6, p7, p7);
    p8 = two_sum(p8, p9, p9);
    /* Compute (t0, t1) = (q0, q3) + (q4, q5). */
    t0 = two_sum(q0, q4, t1);
    t1 += (q3 + q5);
    /* Compute (r0, r1) = (p6, p7) + (p8, p9). */
    r0 = two_sum(p6, p8, r1);
    r1 += (p7 + p9);
    /* Compute (q3, q4) = (t0, t1) + (r0, r1). */
    q3 = two_sum(t0, r0, q4);
    q4 += (t1 + r1);
    /* Compute (t0, t1) = (q3, q4) + s1. */
    t0 = two_sum(q3, s1, t1);
    t1 += q4;

    /* O(eps^4) terms -- Nine-One-Sum */
    t1 += a.y * b.w + a.z * b.z + a.w * b.y + q6 + q7 + q8 + q9 + s2;

    renorm(p0, p1, s0, t0, t1);
    return dvec4(p0, p1, s0, t0);
}

bool greater_than_qd_d(in dvec4 a, in double b)
{
    return a.x > b || (a.x == b && (a.y > 0.0 || (a.y == 0.0 && (a.z > 0.0 || (a.z == 0.0 && a.w > 0.0)))));
}
