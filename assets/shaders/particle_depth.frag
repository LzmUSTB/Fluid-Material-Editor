#version 460 core

in float viewPosZ;
in float particleRadius;
uniform float Near; 
uniform float Far;  
out float fragDepth;

void main() {
    float dist = length(gl_PointCoord*2. - 1.);
    if (dist > 1.) discard;
    float depth = sqrt(1.0 - dist);

    float offset_z = depth * particleRadius;
    float current_z = viewPosZ - offset_z;
    float linearDepth = (2.0*Near*Far)/(Far+Near-current_z*(Far-Near)); 
     
    fragDepth = linearDepth;
}