#version 330 core

// Model Space
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoords;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

uniform mat4 projectionMatrix;
uniform mat4 worldViewMatrix;
uniform mat4 worldMatrix;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vPosition;

void main(void)
{
    gl_Position = projectionMatrix * worldViewMatrix * vec4(position, 1);

    // Forward shading outputs
    vTexCoords = texCoords;
    // Convert to world Space
    vNormal = (projectionMatrix * worldMatrix * vec4(normal,1)).xyz;
    vPosition = (projectionMatrix * worldMatrix * vec4(position,1)).xyz;
}
