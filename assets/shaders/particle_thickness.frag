#version 460 core

in float particleRadius;
out float FragColor;

void main() {
    float dist = length(gl_PointCoord*2. - 1.);
    if (dist > 1.) discard;
    float depth = sqrt(1.0 - dist);
    float thickness = 2.f * depth * particleRadius;
    FragColor = thickness;
}