#version 330 core

layout (location = 0) out vec4 outColor;

in vec2 vTexCoords;

// Geometry info
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;

// Light info
uniform int lightType;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;

uniform vec3 cameraPos;

void main(void)
{
    vec4 color = texture2D(gNormal, vTexCoords);

    outColor = color;
}
