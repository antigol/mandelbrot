#version 130

in vec2 a;

uniform float center[4];
uniform vec3 colormap[512];
uniform int accuracy;
uniform float radius;

out vec4 color;

void main(void)
{
    float cx = center[0] + a.x;
    float cy = center[2] + a.y;

    float x = cx;
    float y = cy;

    int iteration = 0;

    do {
        float x2 = x * x;
        float y2 = y * y;
        if ((x2 + y2) > radius)
            break;

        float xtemp = x2 - y2 + cx;
        y = 2 * x * y + cy;

        x = xtemp;
        ++iteration;
    } while (iteration < accuracy);

    if (iteration < accuracy) {
        color = vec4(colormap[iteration & 511], 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
