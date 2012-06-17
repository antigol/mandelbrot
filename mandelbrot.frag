#version 130

in vec2 a;

uniform vec4 colormap[256];
uniform int accuracy;

out vec4 color;

void main(void)
{
    int MaxIterations = (1 << (2 * accuracy + 6)) + 32;
    const int Limit = 4;


    float a1 = a.x;
    float b1 = a.y;
    int numIterations = 0;

    do {
        ++numIterations;
        float a2 = (a1 * a1) - (b1 * b1) + a.x;
        float b2 = (2 * a1 * b1) + a.y;
        if ((a2 * a2) + (b2 * b2) > Limit)
            break;

        ++numIterations;
        a1 = (a2 * a2) - (b2 * b2) + a.x;
        b1 = (2 * a2 * b2) + a.y;
        if ((a1 * a1) + (b1 * b1) > Limit)
            break;
    } while (numIterations < MaxIterations);

    if (numIterations < MaxIterations) {
        color = colormap[numIterations & 255];
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
