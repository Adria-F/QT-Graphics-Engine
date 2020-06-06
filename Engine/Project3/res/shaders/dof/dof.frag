#version 330 core

uniform sampler2D depth;
uniform sampler2D color;
uniform float depthFocus;

in vec2 texCoord;
out vec4 outColor;

void main(void){
    outColor = texture(color, texCoord);
}
