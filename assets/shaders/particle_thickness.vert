#version 460 core

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 CameraPos;
uniform float ParticleRadius;
uniform float R_screen_coefficient; 

out float particleRadius;
out float viewPosZ;
out float worldDepth;

void main() {
    vec4 pos = positions[gl_InstanceID];

    worldDepth = length(pos.xyz - CameraPos);
    vec4 viewPos = View * pos;
    float R_screen = ParticleRadius * R_screen_coefficient / -viewPos.z;

    particleRadius = ParticleRadius;
    viewPosZ = viewPos.z;
    gl_PointSize = R_screen * 2.;
    gl_Position = Projection * viewPos;
}