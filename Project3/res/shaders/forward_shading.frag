#version 330 core

// Matrices
uniform mat4 worldViewMatrix;
uniform mat3 normalMatrix;

// Material
uniform vec4 albedo;
uniform vec4 specular;
uniform vec4 emissive;
uniform float smoothness;
uniform float bumpiness;
uniform sampler2D albedoTexture;
uniform sampler2D specularTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D normalTexture;
uniform sampler2D bumpTexture;

// Lights
#define MAX_LIGHTS 8
uniform int lightType[MAX_LIGHTS];
uniform vec3 lightPosition[MAX_LIGHTS];
uniform vec3 lightDirection[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform int lightCount;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vPosition;

out vec4 outColor;

void main(void)
{
    // TODO: Local illumination
    // Ambient
    // Diffuse
    // Specular
    vec3 ambient = vec3(0.2f);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    for (int i = 0; i < lightCount; ++i)
    {
        vec3 ray = normalize(lightPosition[i]-vPosition);
        if (lightType[i] == 1)
            ray = lightDirection[i];

        diffuse += max(dot(ray, vNormal),0.0)*lightColor[i];
        if (length(diffuse) > 0.0)
        {
            vec3 R = reflect(-ray, vNormal); //Reflected light vector
            vec3 V = normalize(vec3(0.0f)-vPosition); //Vector to viewer

            float specFactor = max(dot(R,V),0.0);
            specular += pow(specFactor, 32.0f)*lightColor[i];
        }
    }

    outColor.rgb = texture(albedoTexture, vTexCoords).rgb*(ambient+diffuse+specular);
    outColor.rgb = vNormal;
}
