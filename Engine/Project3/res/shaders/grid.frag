#version 330 core

uniform float left;
uniform float right;
uniform float bottom;
uniform float top;
uniform float znear;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

in vec2 texCoord;
out vec4 outColor;


float grid(vec3 worldPos, float gridStep){
    vec2 grid = fwidth(worldPos.xz)/mod(worldPos.xz, gridStep);
    float line = step(1.0, max(grid.x, grid.y));
    return line;
}

void main(void)
{
    outColor = vec4(1.0);

    // Eye direction
    vec3 eyedirEyespace;
    eyedirEyespace.x = left + texCoord.x * (right-left);
    eyedirEyespace.y = bottom + texCoord.y  * (top - bottom);
    eyedirEyespace.z = -znear;
    vec3 eyedirWorldspace = normalize(mat3(worldMatrix) * eyedirEyespace);

    // Eye position
    vec3 eyeposEyespace = vec3(0.0);
    vec3 eyeposWorldspace = vec3(worldMatrix* vec4(eyeposEyespace, 1.0));

    // Plane parameters
    vec3 planeNormalWorldspace = vec3(0.0, 1.0, 0.0);
    vec3 planePointWorldspace = vec3(0.0);

    // Rayplane intersection
    float numerator = dot(planePointWorldspace - eyeposWorldspace, planeNormalWorldspace);
    float denominator = dot(eyedirWorldspace, planeNormalWorldspace);
    float t = numerator/denominator;

    if(t > 0.0){
        vec3 hitWorldspace = eyeposWorldspace + eyedirWorldspace * t;

        if(grid(hitWorldspace, 1.0) == 1.0){
            outColor = vec4(vec3(1.0), 0.2);
            if(grid(hitWorldspace, 10.0) == 1.0)
                outColor.a = 0.5;

            outColor.a *= clamp((20.0 /t), 0.0, 1.0);
        }
        else
            discard;

    }
    else{
        gl_FragDepth = 0.0;
        discard;
    }
}
