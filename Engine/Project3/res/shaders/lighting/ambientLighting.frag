#version 330 core

out vec4 ambientLight;

uniform sampler2D gColor;
uniform sampler2D ssao;

uniform float ambientValue;

void main(void)
{
    vec2 viewportSize = textureSize(gColor, 0);
    vec2 texCoords = gl_FragCoord.xy/viewportSize;

    ambientLight = texture2D(gColor, texCoords) * texture2D(ssao, texCoords) * ambientValue;
}
