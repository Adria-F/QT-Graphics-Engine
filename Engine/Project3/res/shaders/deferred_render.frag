#version 330 core


uniform sampler2D Depth;
in vec3 vNormal;

layout (location = 0) out vec3 normals;
layout (location = 1) out vec3 depth;

void main(void)
{
    normals = vNormal;
    depth = vec3(0,1,0);
}
