#version 460 core

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

uniform mat4 View;
uniform mat4 Projection;
uniform float ParticleRadius;

void main() {
    vec3 pos = positions[gl_InstanceID].xyz;
    gl_Position = Projection * View * vec4(pos, 1.0);
}