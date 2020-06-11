#version 330 core

uniform sampler2D depth;
uniform sampler2D color;
uniform float depthFocus;
uniform vec2 viewportSize;
uniform vec2 texCoordInc;



in vec2 texCoord;
out vec4 outColor;

// Should be uniforms
float near = 0.01;
float far = 10000.0;
// From depthFocus to start
float fallofStartMargin = 3;
// From depthFocus to end
float fallofEndMargin = 10;

float LinearizeDepth(float rawDepth){
    float z = rawDepth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));

}

void main(void){

    outColor = texture(color, texCoord);

    // DOF is disabled
    if(depthFocus < 0)
        return;

    float pixelDepth = LinearizeDepth(texture(depth, texCoord).r);


    float depthDiff = abs(pixelDepth - depthFocus);

    float blurCoeficient = 1.0f;

    // If the pixel is within the start margin, don't blur at all
    if(depthDiff < fallofStartMargin)
        return;
    // If outside, we compute the blurCoeficient with fallofEndMargin
    else{
        blurCoeficient = clamp(depthDiff / fallofEndMargin, 0.0, 1.0);
    }



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
        float currentWeight = weights[i] * blurCoeficient;
        blurredColor += texture(color, uv).rgb * currentWeight;
        sumWeights += currentWeight;
        uv += pixelInc;
    }

    blurredColor /= sumWeights;


    outColor = vec4(blurredColor, 1.0);
}
