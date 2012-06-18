#version 420

in vec2 a;

uniform dvec2 center;
uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

void main(void)
{
    const float Limit = 4.0;

    double x = 0.0;
    double y = 0.0;
    int iteration = 0;

    do {
        double x2 = x * x;
        double y2 = y * y;
        if ((x2 + y2) > 4.0)
            break;
        double xtemp = x2 - y2 + a.x + center.x;
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
