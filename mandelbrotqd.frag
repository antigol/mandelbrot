#version 420

in vec2 a;

uniform dvec2 center[4];
uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

struct qd_real {
  double x[4];
};
qd_real qd_real_mk(in double c0, in double c1, in double c2, in double c3);
qd_real add_qd_d(in qd_real a, in double b);
qd_real mul_qd_d(in qd_real a, in double b);
qd_real add_qd_qd(in qd_real a, in qd_real b);
qd_real mul_qd_qd(in qd_real a, in qd_real b);
qd_real minus_qd(in qd_real a);

void main(void)
{
    const float Limit = 4.0;

    qd_real cx = qd_real_mk(center[0].x, center[1].x, center[2].x, center[3].x);
    qd_real cy = qd_real_mk(center[0].y, center[1].y, center[2].y, center[3].y);
//    qd_real cx = qd_real_mk(center.x, 0.0, 0.0, 0.0);
//    qd_real cy = qd_real_mk(center.y, 0.0, 0.0, 0.0);

    cx = add_qd_d(cx, a.x);
    cy = add_qd_d(cy, a.y);

    qd_real x;
    x.x[0] = x.x[1] = x.x[2] = x.x[3] = 0.0;

    qd_real y;
    y.x[0] = y.x[1] = y.x[2] = y.x[3] = 0.0;

    int iteration = 0;

    do {
        qd_real x2 = mul_qd_qd(x, x);
        qd_real y2 = mul_qd_qd(y, y);
        qd_real lt = add_qd_qd(x2, y2);
        if (lt.x[0] > 4.0 || (lt.x[0] == 4.0 && lt.x[1] > 0.0))
            break;

        qd_real xtemp = add_qd_qd(x2, minus_qd(y2));
        xtemp = add_qd_qd(xtemp, cx);

        y = mul_qd_qd(x, y);
        y = mul_qd_d(y, 2.0);
        y = add_qd_qd(y, cy);

        x = xtemp;
        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = colormap[iteration & 255];
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

qd_real qd_real_mk(in double c0, in double c1, in double c2, in double c3)
{
    qd_real c;
    c.x[0] = c0;
    c.x[1] = c1;
    c.x[2] = c2;
    c.x[3] = c3;
    return c;
}

qd_real add_qd_d(in qd_real a, in double b)
{
    double c0, c1, c2, c3;
    double e;

    c0 = two_sum(a.x[0], b, e);
    c1 = two_sum(a.x[1], e, e);
    c2 = two_sum(a.x[2], e, e);
    c3 = two_sum(a.x[3], e, e);

    renorm(c0, c1, c2, c3, e);

    return qd_real_mk(c0, c1, c2, c3);
}

void split(in double a, out double hi, out double lo)
{
    const double _QD_SPLITTER = 134217729.0;               // = 2^27 + 1
//    const double _QD_SPLIT_THRESH = 6.69692879491417e+299; // = 2^996

    double temp;
//    if (a > 6.69692879491417e+299 || a < -6.69692879491417e+299) {
//      a *= 3.7252902984619140625e-09;  // 2^-28
//      temp = _QD_SPLITTER * a;
//      hi = temp - (temp - a);
//      lo = a - hi;
//      hi *= 268435456.0;          // 2^28
//      lo *= 268435456.0;          // 2^28
//    } else {
      temp = _QD_SPLITTER * a;
      hi = temp - (temp - a);
      lo = a - hi;
//    }
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

qd_real mul_qd_d(in qd_real a, in double b)
{
    double p0, p1, p2, p3;
    double q0, q1, q2;
    double s0, s1, s2, s3, s4;

    p0 = two_prod(a.x[0], b, q0);
    p1 = two_prod(a.x[1], b, q1);
    p2 = two_prod(a.x[2], b, q2);
    p3 = a.x[3] * b;

    s0 = p0;

    s1 = two_sum(q0, p1, s2);

    three_sum(s2, q1, p2);

    three_sum2(q1, q2, p3);
    s3 = q1;

    s4 = q2 + p2;

    renorm(s0, s1, s2, s3, s4);
    return qd_real_mk(s0, s1, s2, s3);
}

qd_real add_qd_qd(in qd_real a, in qd_real b)
{
    double s0, s1, s2, s3;
    double t0, t1, t2, t3;

    double v0, v1, v2, v3;
    double u0, u1, u2, u3;
    double w0, w1, w2, w3;

    s0 = a.x[0] + b.x[0];
    s1 = a.x[1] + b.x[1];
    s2 = a.x[2] + b.x[2];
    s3 = a.x[3] + b.x[3];

    v0 = s0 - a.x[0];
    v1 = s1 - a.x[1];
    v2 = s2 - a.x[2];
    v3 = s3 - a.x[3];

    u0 = s0 - v0;
    u1 = s1 - v1;
    u2 = s2 - v2;
    u3 = s3 - v3;

    w0 = a.x[0] - u0;
    w1 = a.x[1] - u1;
    w2 = a.x[2] - u2;
    w3 = a.x[3] - u3;

    u0 = b.x[0] - v0;
    u1 = b.x[1] - v1;
    u2 = b.x[2] - v2;
    u3 = b.x[3] - v3;

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
    return qd_real_mk(s0, s1, s2, s3);
}

qd_real mul_qd_qd(in qd_real a, in qd_real b)
{
    double p0, p1, p2, p3, p4, p5;
    double q0, q1, q2, q3, q4, q5;
    double p6, p7, p8, p9;
    double q6, q7, q8, q9;
    double r0, r1;
    double t0, t1;
    double s0, s1, s2;

    p0 = two_prod(a.x[0], b.x[0], q0);

    p1 = two_prod(a.x[0], b.x[1], q1);
    p2 = two_prod(a.x[1], b.x[0], q2);

    p3 = two_prod(a.x[0], b.x[2], q3);
    p4 = two_prod(a.x[1], b.x[1], q4);
    p5 = two_prod(a.x[2], b.x[0], q5);

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
    p6 = two_prod(a.x[0], b.x[3], q6);
    p7 = two_prod(a.x[1], b.x[2], q7);
    p8 = two_prod(a.x[2], b.x[1], q8);
    p9 = two_prod(a.x[3], b.x[0], q9);

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
    t1 += a.x[1] * b.x[3] + a.x[2] * b.x[2] + a.x[3] * b.x[1] + q6 + q7 + q8 + q9 + s2;

    renorm(p0, p1, s0, t0, t1);
    return qd_real_mk(p0, p1, s0, t0);
}

qd_real minus_qd(in qd_real a)
{
    return qd_real_mk(-a.x[0], -a.x[1], -a.x[2], -a.x[3]);
}
