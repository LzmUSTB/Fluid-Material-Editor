#version 460 core

in float viewPosZ;
in float particleRadius;
uniform mat4 Projection;
uniform float Near; 
uniform float Far;  
out float fragDepth;

void main() {
    vec2 r = gl_PointCoord*2. - 1.;
    float dist = dot(r,r);
    if (dist > 1.) discard;
    float depth = sqrt(1.0 - dist);

    float surfaceViewZ = viewPosZ + depth * particleRadius;
    vec4 clipPos = Projection * vec4(0.0, 0.0, surfaceViewZ, 1.0);
    float ndcDepth = clipPos.z / clipPos.w;

    gl_FragDepth = ndcDepth * 0.5 + 0.5;
    fragDepth = -surfaceViewZ;
}
