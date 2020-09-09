#version 330 core
precision mediump float;

in vec3 pNormal;
in vec4 pColor;
in vec2 pTexCoord;

out vec4 FragColor;

uniform vec3 SunDirection;
uniform vec3 SunColor;
uniform float SunIntensity;

uniform sampler2D Texture;
uniform vec2 Tiling;

void main()
{
    vec4 albedo = texture(Texture, pTexCoord * Tiling) * pColor;
    vec3 radiosity = albedo.rgb * SunColor;

    vec3 ambient = radiosity * pow(SunIntensity * 0.1, 0.5);

    float illumination = max(dot(normalize(pNormal), -SunDirection), 0);
    vec3 diffuse = radiosity * SunIntensity * illumination;

    vec3 color = ambient + diffuse;
    
    int cutout = int(albedo.a >= 0.5);
    gl_FragDepth = (cutout * gl_FragCoord.z) + (1 - cutout);

    FragColor = vec4(color, 1.0);
};