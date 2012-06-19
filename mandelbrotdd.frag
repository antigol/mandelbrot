#version 420

in vec2 a;

uniform dvec2 center[4];
uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

struct qd_real {
  double x[4];
};
qd_real add_qd_d(qd_real a, double b);
qd_real mul_qd_d(qd_real a, double b);
qd_real add_qd_qd(qd_real a, qd_real b);
qd_real mul_qd_qd(qd_real a, qd_real b);
qd_real minus_qd(qd_real a);

void main(void)
{
    const float Limit = 4.0;

    qd_real cx;
    cx.x[0] = center[0].x;
    cx.x[1] = center[1].x;
    cx.x[2] = center[2].x;
    cx.x[3] = center[3].x;
    qd_real cy;
    cx.x[0] = center[0].y;
    cx.x[1] = center[1].y;
    cx.x[2] = center[2].y;
    cx.x[3] = center[3].y;

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

qd_real add_qd_d(qd_real a, double b);
qd_real mul_qd_d(qd_real a, double b);
qd_real add_qd_qd(qd_real a, qd_real b);
qd_real mul_qd_qd(qd_real a, qd_real b);
qd_real minus_qd(qd_real a)
{

}
