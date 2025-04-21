#version 460 core

uniform sampler2D LinearDepthMap; 
uniform float near;               
uniform float fov;                
uniform float aspectRatio;      

out vec4 FragColor;

vec3 getViewPos(vec2 uv, float z) {
    float x = (uv.x * 2.0 - 1.0);
    float y = (uv.y * 2.0 - 1.0);

    float tanHalfFov = tan(fov * 0.5);
    float viewX = x * z * tanHalfFov * aspectRatio;
    float viewY = y * z * tanHalfFov;
    return vec3(viewX, viewY, -z); 
}

void main() {
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(LinearDepthMap, 0);
    vec2 texel = 1.0 / textureSize(LinearDepthMap, 0);

    float depthC = texture(LinearDepthMap, uv).r;
    float depthR = texture(LinearDepthMap, uv + vec2(texel.x, 0)).r;
    float depthU = texture(LinearDepthMap, uv + vec2(0, texel.y)).r;

    vec3 posC = getViewPos(uv, depthC);
    vec3 posR = getViewPos(uv + vec2(texel.x, 0), depthR);
    vec3 posU = getViewPos(uv + vec2(0, texel.y), depthU);

    vec3 dx = posR - posC;
    vec3 dy = posU - posC;

    vec3 normal = normalize(cross(dy, dx)); 

    FragColor = vec4(normal, 1.0); 
}
