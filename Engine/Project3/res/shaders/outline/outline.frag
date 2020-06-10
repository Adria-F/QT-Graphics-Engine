#version 330 core

out vec4 outColor;

uniform sampler2D mask;

void main(void)
{
    vec2 viewportSize = textureSize(mask, 0);
    vec2 texCoords = gl_FragCoord.xy/viewportSize;
    vec2 texInc = vec2(1.0)/viewportSize;
    vec2 incx = vec2(texInc.x, 0.0);
    vec2 incy = vec2(0.0, texInc.y);
    float c = texture(mask, texCoords).r;
    float l = texture(mask, texCoords-incx).r;
    float r = texture(mask, texCoords+incx).r;
    float b = texture(mask, texCoords-incy).r;
    float t = texture(mask, texCoords+incy).r;

    bool outline = c < 0.1 && (l > 0.1 || r > 0.1 || b > 0.1 || t > 0.1);

    if (outline == false) { discard; }

    outColor = vec4(1.0,0.5,0.0,1.0);
}
