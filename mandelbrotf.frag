#version 130

in vec2 a;

uniform vec2 center;
uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

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
        const float loglogb = log(log(2.0f));
        const float invlog2 = 1.0f / log(2.0f);
        float v = (loglogb - log(log(sqrt(x * x + y * y)))) * invlog2;
        color = mix(colormap[iteration & 255], colormap[(iteration+1) & 255], v);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
