#version 330 core

layout (location = 0) out vec4 finalRender;
layout (location = 1) out vec4 lightCircles;

in vec4 gl_FragCoord;

// Geometry info
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;

// Light info
uniform int lightType;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform float lightRange;

uniform vec3 cameraPos;
uniform vec2 viewportSize;

void main(void)
{
    vec2 pixelCoords= gl_FragCoord.xy/viewportSize;

    vec3 position = texture2D(gPosition, pixelCoords).xyz;
    vec3 normal = texture2D(gNormal, pixelCoords).xyz;
    vec3 color = texture2D(gColor, pixelCoords).xyz;

    lightCircles.rgb = lightColor;

    float pointDst = length(position-lightPosition);
    if (lightType == 0 && pointDst > lightRange)
        discard;

    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    vec3 ray = normalize(lightPosition-position);
    if (lightType == 1)
        ray = lightDirection;

    diffuse += max(dot(ray, normal),0.0f);
    if (length(diffuse) > 0.0f)
    {
        vec3 R = reflect(-ray, normal); //Reflected light vector
        vec3 V = normalize(cameraPos-position); //Vector to viewer

        float specFactor = max(dot(R,V),0.0f);
        specular += pow(specFactor, 32.0f);
    }

    float attenuation = pow((1.0f-pointDst/lightRange),2.0f);
    if (lightType == 1)
        attenuation = 1.0f;
    finalRender.rgb = color*(diffuse+specular)*lightIntensity*attenuation*lightColor;
}
