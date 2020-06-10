#version 330 core

layout(location=0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 worldViewMatrix;

void main(void)
{
    gl_Position = projectionMatrix * worldViewMatrix * vec4(position, 1);
}
