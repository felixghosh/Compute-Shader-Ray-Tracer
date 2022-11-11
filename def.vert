#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;

uniform float time;

out vec3 fColor;

void main()
{
    gl_Position = vec4(aPos.x + sin(time)*1.1, aPos.y + sin(time)*1.1, aPos.z + sin(time)*1.1, 1.0);
    fColor = color;
}