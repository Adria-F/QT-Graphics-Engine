#version 330 core


uniform float left;
uniform float right;
uniform float bottom;
uniform float top;
uniform float znear;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// Background
uniform vec2 viewportSize;
uniform vec4 backgroundColor;

// Models depth
uniform sampler2D depth;

in vec2 texCoord;
out vec4 outColor;


float grid(vec3 worldPos, float gridStep){
    vec2 grid = fwidth(worldPos.xz)/mod(worldPos.xz, gridStep);
    float line = step(1.0, max(grid.x, grid.y));
    return line;
}

vec4 computeBackgroundColor(){
    vec2 pixelCoord = gl_FragCoord.xy / viewportSize;
    vec3 rayViewspace = normalize(vec3(vec2(left, bottom) + pixelCoord * vec2(right - left, top - bottom), -znear));
    vec3 rayWorldspace = vec3(worldMatrix * vec4(rayViewspace, 0.0));
    vec3 horizonWorldspace = rayWorldspace * vec3(1.0, 0.0, 1.0);
    float elevation = (1.0 - dot(rayWorldspace, horizonWorldspace)) * sign(rayWorldspace.y);

    vec3 bgColor = pow(backgroundColor.rgb, vec3(2,2,2));
    float bgIntensity = 0.1 * bgColor.r + 0.7 * bgColor.g + 0.2 * bgColor.b;
    float skyFactor = smoothstep(0.0, 1.0, elevation);
    float groundFactor = smoothstep(0.0, -0.0005, elevation);
    float horizonFactor = clamp(1.0 - max(skyFactor, groundFactor), 0.0, 1.0);
    vec3 horizonColor = horizonFactor * bgColor;
    vec3 groundColor = groundFactor * vec3(bgIntensity) * 0.2;
    vec3 skyColor = skyFactor * bgColor * 0.7;
    outColor.rgb = groundColor + skyColor + horizonColor;
    outColor.a = 1.0;

    return outColor;

}

void main(void)
{
    outColor = computeBackgroundColor();
    float fragmentDepth = texture(depth, gl_FragCoord.xy / viewportSize).r;

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

        // Small bias to avoid zfighting
        vec3 bias = (eyeposWorldspace - hitWorldspace) * 0.005;
        // Compute grid depth
        vec4 hitClip = projectionMatrix * viewMatrix * vec4(hitWorldspace + bias, 1.0);
        float ndcDepth = hitClip.z / hitClip.w;
        float gridDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0f;

        // If this part of the grid is behind a model, discard it
        if(gridDepth > fragmentDepth)
            discard;

        vec4 gridColor = outColor;

        if(grid(hitWorldspace, 1.0) == 1.0)
            gridColor = vec4(vec3(1.0), 0.3);
        if(grid(hitWorldspace, 10.0) == 1.0)
            gridColor.a = 1.0;

        // Fade out the furthest parts of the gird
        if(gridColor != outColor)
            gridColor.a *= clamp((20.0 /t), 0.0, 1.0);
        // If there is a model discard the background
        else if(fragmentDepth < 1.0)
            discard;


        outColor = gridColor;
    }else if(fragmentDepth < 1.0){
        // If there is a model discard the background
        discard;
    }
}
