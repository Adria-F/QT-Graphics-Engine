#version 330 core

uniform sampler2D albedoTexture;
uniform sampler2D Depth;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoords;

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 color;

void main(void)
{
    position.rgb = vPosition;
    normal.rgb = normalize(vNormal);
    color.rgb = texture(albedoTexture, vTexCoords).rgb;
}
