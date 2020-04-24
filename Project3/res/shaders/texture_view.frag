#version 330 core

uniform sampler2D colorMap;

in vec2 outTexCoord;

out vec4 outColor;

void main(void)
{
    //outColor = vec4(outTexCoord.r, outTexCoord.g, 1.0, 1.0);
    outColor = texture(colorMap, outTexCoord);
    outColor.a = 1.0;

    // Gamma correction
    outColor.rgb = pow(outColor.rgb, vec3(1.0/2.4));
}

