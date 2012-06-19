#version 420

in vec2 a;

uniform vec2 center;
uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

struct FPN
{
    int n0; // avant la virgule
    uint n1, n2, n3;
};

// add, mul, inv

void main(void)
{
    const float Limit = 4.0;

    float x = 0.0;
    float y = 0.0;
    int iteration = 0;

    do {
        float x2 = x * x;
        float y2 = y * y;
        if ((x2 + y2) > 4.0)
            break;

        float xtemp = x2 - y2 + a.x + center.x;
        y = 2 * x * y + a.y + center.y;
        x = xtemp;
        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = colormap[iteration & 255];
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

void fpnadd2positive(FPN a, in FPN b, out FPN c)
{
    // additionne exclusivement deux nombres positifs entre eux
    // il faut regarder s'il y a eu dépassement (seul les 31 permier bits sont utilisée)
    c.n3 = a.n3 + b.n3;
    if (c.n3 & 0x80000000) {
        c.n3 -= 0x80000000;
        a.n2 += 1;
    }

    c.n2 = a.n2 + b.n2;
    if (c.n2 & 0x80000000) {
        c.n2 -= 0x80000000;
        a.n1 += 1;
    }

    c.n1 = a.n1 + b.n1;
    if (c.n1 & 0x80000000) {
        c.n1 -= 0x80000000;
        a.n0 += 1;
    }

    c.n0 += a.n0 + b.n0;
}

void fpnsub2positive(FPN a, in FPN b, out FPN c)
{
    c.n3 = a.n3 - b.n3;
    if (c.n3 & 0x80000000) {
        c.n3 += 0x80000000;
        a.n2 -= 1;
    }

    c.n2 = a.n2 - b.n2;
    if (c.n2 & 0x80000000) {
        c.n2 += 0x80000000;
        a.n1 -= 1;
    }

    c.n1 = a.n1 - b.n1;
    if (c.n1 & 0x80000000) {
        c.n1 += 0x80000000;
        a.n0 -= 1;
    }

    c.n0 = a.n0 - b.n0;
}
