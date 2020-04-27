#version 330 core

layout(location=0) in vec3 position;

uniform mat4 projectionMatrix;
uniform vec3 scale;

out vec2 outTexCoord;

void main(void)
{
    gl_Position = projectionMatrix * vec4(position * scale, 1.0);
    outTexCoord = position.xy + vec2(0.5);
}
