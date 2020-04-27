#version 330 core

uniform sampler2D colorTexture;
uniform bool blitAlpha;
uniform bool blitDepth;

in vec2 texCoord;

out vec4 outColor;

void main(void)
{
    vec4 texel = texture(colorTexture, texCoord);

    if (blitAlpha) {
        outColor.rgb = vec3(texel.a);
    } else if (blitDepth) {
        float f = 10000.0;
        float n = 0.01;
        float z = abs((2 * f * n) / ((texel.r * 2.0 - 1.0) *(f-n)-(f+n)));
        outColor.rgb = vec3(z / 50.0);
    } else {
        outColor.rgb = texel.rgb;
    }

    // Gamma correction
    outColor = pow(outColor, vec4(1.0/2.2));
    outColor.a = 1.0;
}
