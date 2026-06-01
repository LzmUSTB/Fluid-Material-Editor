#version 460 core

uniform sampler2D LinearDepthMap; 
uniform float near;               
uniform float fov;                
uniform float aspectRatio;      
uniform mat4x4 inv_View;
uniform mat4x4 inv_Projection;


out vec4 FragColor;

bool isValidDepth(float depth) {
    return depth > 1e-5 && depth < 1e6;
}

float readDepth(vec2 uv) {
    return texture(LinearDepthMap, clamp(uv, vec2(0.0), vec2(1.0))).r;
}

float readDepthOrCenter(vec2 uv, float centerDepth) {
    float depth = readDepth(uv);
    float maxJump = max(0.08, centerDepth * 0.025);
    if (!isValidDepth(depth) || abs(depth - centerDepth) > maxJump) {
        return centerDepth;
    }
    return depth;
}

vec3 getViewPosFromDepth(vec2 uv, float depth) {
    float x = uv.x * 2.0 - 1.0;
    float y = -(uv.y * 2.0 - 1.0);
    float tanHalfFov = tan(fov * 0.5);
    return vec3(
        x * depth * tanHalfFov * aspectRatio,
        y * depth * tanHalfFov,
        -depth
    );
}

vec3 normalFromStep(vec2 uv, float centerDepth, vec2 stepSize) {
    float depthR = readDepthOrCenter(uv + vec2(stepSize.x, 0.0), centerDepth);
    float depthL = readDepthOrCenter(uv - vec2(stepSize.x, 0.0), centerDepth);
    float depthU = readDepthOrCenter(uv + vec2(0.0, stepSize.y), centerDepth);
    float depthD = readDepthOrCenter(uv - vec2(0.0, stepSize.y), centerDepth);

    vec3 posR = getViewPosFromDepth(uv + vec2(stepSize.x, 0.0), depthR);
    vec3 posL = getViewPosFromDepth(uv - vec2(stepSize.x, 0.0), depthL);
    vec3 posU = getViewPosFromDepth(uv + vec2(0.0, stepSize.y), depthU);
    vec3 posD = getViewPosFromDepth(uv - vec2(0.0, stepSize.y), depthD);

    vec3 dx = posR - posL;
    vec3 dy = posU - posD;
    return cross(dy, dx);
}

void main() {
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(LinearDepthMap, 0);
    vec2 texel = 1.0 / textureSize(LinearDepthMap, 0);

    float depthC = readDepth(uv);
    if (!isValidDepth(depthC)) {
        FragColor = vec4(0.0);
        return;
    }

    vec3 n = normalFromStep(uv, depthC, texel) * 0.65;
    n += normalFromStep(uv, depthC, texel * 2.0) * 0.35;

    if (dot(n, n) <= 1e-12) {
        FragColor = vec4(0.0);
        return;
    }

    vec3 normal = normalize(n); 
    FragColor = vec4(normal, 1.0); 
}
