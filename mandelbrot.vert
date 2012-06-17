#version 130

in vec2 vertex;

uniform vec2 center;
uniform float scale; // height
uniform float ratio;

out vec2 a;

void main(void)
{
    a = center + vec2(vertex.x * scale * ratio, vertex.y * scale);
    gl_Position = vec4(vertex, 0.0, 1.0);
}
