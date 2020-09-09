#version 330 core
precision mediump float;

in vec2 pPos;

out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, pPos);
};