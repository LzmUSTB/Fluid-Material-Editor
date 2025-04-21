#version 460 core

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

uniform mat4 View;
uniform mat4 Projection;
uniform float ParticleRadius;

void main() {
    vec4 pos = positions[gl_InstanceID];
                                                                
    gl_Position = Projection * View * vec4(pos);
}