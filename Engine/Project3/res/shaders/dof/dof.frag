#version 330 core

uniform sampler2D depth;
uniform sampler2D color;
uniform float depthFocus;
uniform vec2 viewportSize;
uniform vec2 texCoordInc;


in vec2 texCoord;
out vec4 outColor;

void main(void){

    float weights[11];
    weights[0] = 0.035822;
    weights[1] = 0.05879;
    weights[2] = 0.086425;
    weights[3] = 0.113806;
    weights[4] = 0.13424;
    weights[5] = 0.141836;
    weights[6] = 0.13424;
    weights[7] = 0.113806;
    weights[8] = 0.086425;
    weights[9] = 0.05879;
    weights[10] = 0.035822;

    //Uniform
    vec2 pixelInc = texCoordInc / viewportSize;
    vec3 blurredColor = vec3(0.0);
    vec2 uv = texCoord - pixelInc * 5.0;
    float sumWeights = 0.0f;
    for(int i = 0; i < 11; ++i){
        blurredColor += texture(color, uv).rgb * weights[i];
        sumWeights += weights[i];
        uv += pixelInc;
    }

    blurredColor /= sumWeights;


    outColor = vec4(blurredColor, 1.0);
}
