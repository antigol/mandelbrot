#version 420

in vec2 a;

uniform double center[4];
uniform vec3 colormap[256];
uniform int accuracy;
uniform float radius;

out vec4 color;

void main(void)
{
    double cx = center[0] + double(a.x);
    double cy = center[2] + double(a.y);

    double x = cx;
    double y = cy;

    int iteration = 0;

    do {
        double x2 = x * x;
        double y2 = y * y;
        if ((x2 + y2) > radius)
            break;

        y = 2 * x * y + cy;
        x = x2 - y2 + cx;

        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = vec4(colormap[iteration & 255], 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
