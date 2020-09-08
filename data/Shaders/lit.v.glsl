#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNor;
layout (location = 2) in vec4 vCol;
layout (location = 3) in vec2 vTex;

out vec3 pNormal;
out vec4 pColor;
out vec2 pTexCoord;

uniform mat4 Model;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    pNormal = mat3(Model) * vNor;
    pColor = vCol;
    pTexCoord = vTex;
}