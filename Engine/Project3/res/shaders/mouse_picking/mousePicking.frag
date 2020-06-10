#version 330 core

layout (location = 0) out vec3 finalColor;

uniform vec3 colorId;

void main(void)
{
    finalColor = colorId;
}
