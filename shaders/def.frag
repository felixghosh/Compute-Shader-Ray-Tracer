#version 450 core
in vec3 fColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(fColor.r, fColor.g, fColor.b, 1.0f);
} 