#version 460 core

uniform sampler2D LinearDepthMap; 
uniform float near;               
uniform float fov;                
uniform float aspectRatio;      
uniform mat4x4 inv_View;
uniform mat4x4 inv_Projection;


out vec4 FragColor;

vec3 getViewPos(vec2 uv) {
    float x = (uv.x * 2.0 - 1.0);
    float y = -(uv.y * 2.0 - 1.0);
    float depth = texture(LinearDepthMap, uv).r;

    //float tanHalfFov = tan(fov * 0.5);
    //float viewX = x * z * tanHalfFov * aspectRatio;
    //loat viewY = y * z * tanHalfFov;
    vec4 ndc = vec4(x,y,0,1);
    ndc.z = -inverse(inv_Projection)[2].z + inverse(inv_Projection)[3].z / depth;
    vec4 viewPos = inv_Projection * ndc;
    return viewPos.xyz / viewPos.w; 
}

void main() {
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(LinearDepthMap, 0);
    vec2 texel = 1.0 / textureSize(LinearDepthMap, 0);

    vec3 posC = getViewPos(uv);
    vec3 posR = getViewPos(uv + vec2(texel.x, 0));
    vec3 posL = getViewPos(uv - vec2(texel.x, 0));
    vec3 posU = getViewPos(uv + vec2(0, texel.y));
    vec3 posD = getViewPos(uv - vec2(0, texel.y));

    vec3 dx = posR - posC;
    vec3 dx2 = posC - posL;
    vec3 dy = posU - posC;
    vec3 dy2 = posC - posD;
    if(abs(dx2.z)<abs(dx.z)) {dx = dx2;}
    if(abs(dy2.z)<abs(dy.z)) {dy = dy2;}
    
    vec3 normal = normalize(cross(dy, dx)); 
    vec3 visualNormal = normal * 0.5 + 0.5;

    mat3 normalMatrix = transpose(mat3(inv_View));
    vec3 worldNormal = normalize(normalMatrix * normal);
    FragColor = vec4(normal, 1.0); 
}
