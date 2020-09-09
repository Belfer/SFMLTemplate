#version 330 core

layout (location = 0) in vec2 vPos;

out vec2 pPos;

void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
    pPos = vPos;
}