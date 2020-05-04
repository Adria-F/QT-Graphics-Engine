#version 330 core


uniform sampler2D Depth;
in vec3 vNormal;

layout (location = 0) out vec4 normals;
layout (location = 1) out vec4 depth;

void main(void)
{
    normals.rgb = vNormal;
    depth.rgb = vec3(0,1,0);
}
