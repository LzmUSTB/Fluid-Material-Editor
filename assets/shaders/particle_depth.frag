#version 460 core

in float viewPosZ;
in float particleRadius;
in float worldDepth;
uniform float Near; 
uniform float Far;  
out float fragDepth;

void main() {
    vec2 r = gl_PointCoord*2. - 1.;
    float dist = dot(r,r);
    if (dist > 1.) discard;
    float depth = sqrt(1.0 - dist);

    float offset_z = depth * particleRadius;
    float current_z = viewPosZ - offset_z;
    //float linearDepth = worldDepth - offset_z;
   float linearDepth = (2.0*Near*Far)/(Far+Near-current_z*(Far-Near)); 
    fragDepth = linearDepth;
}