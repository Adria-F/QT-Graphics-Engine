#version 330 core



uniform sampler2D albedoTexture;
uniform sampler2D Depth;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoords;


layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normals;
layout (location = 2) out vec4 colors;


void main(void)
{
    position.rgb = vPosition;
    normals.rgb = normalize(vNormal);
    colors.rgb = texture(albedoTexture, vTexCoords).rgb;
}
