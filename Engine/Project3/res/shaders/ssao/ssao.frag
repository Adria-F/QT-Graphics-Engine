#version 330 core

out vec4 outColor;

// Geometry info
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D noiseMap;

uniform vec3 samples[64];
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(void)
{
    vec2 viewportSize = textureSize(gPosition, 0);
    vec2 texCoords = gl_FragCoord.xy/viewportSize;

    vec3 fragPos = (viewMatrix * vec4(texture(gPosition, texCoords).xyz, 1.0)).xyz;
    vec3 normal = (viewMatrix * texture(gNormal, texCoords)).xyz;

    //Avoid banding patterns
    vec2 noiseScale = viewportSize / textureSize(noiseMap, 0);
    vec3 randomVec = texture(noiseMap, texCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for (int i = 0; i < 64; ++i)
    {
        //convert sample offset from tangent to view space
        vec3 samplePos = TBN * samples[i];
        //get neighboring sample position in view space
        samplePos = fragPos + samplePos; // 0.5 == radius of ssao

        //project the sample position to texture coordinates
        vec4 sampleTexCoords = projectionMatrix * vec4(samplePos, 1.0);
        sampleTexCoords.xyz /= sampleTexCoords.w;
        sampleTexCoords.xyz = sampleTexCoords.xyz * 0.5 + 0.5;

        //texture look-up on the depthmap and reconstruct the sampled position      
        float sampleDepth = texture(gDepth, sampleTexCoords.xy).r;
        float xndc = sampleTexCoords.x * 2.0 - 1.0;
        float yndc = sampleTexCoords.y * 2.0 - 1.0;
        float zndc = sampleDepth * 2.0 - 1.0;
        vec4 posNDC = vec4(xndc, yndc, zndc, 1.0);
        vec4 posView = inverse(projectionMatrix) * posNDC;
        vec3 sampledPos = posView.xyz/posView.w;

        //Fix occlusion of distant objects
        float rangeCheck = smoothstep(0.0, 1.0, 0.5 / abs(samplePos.z - sampledPos.z));
        rangeCheck *= rangeCheck;

        //sum occlusion
        occlusion += (samplePos.z < sampledPos.z - 0.02 ? 1.0 : 0.0) * rangeCheck;
    }

    outColor = vec4(1.0 - occlusion / 64.0);
}
