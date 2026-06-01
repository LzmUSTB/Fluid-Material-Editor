#version 460 core

in float particleRadius;
out float FragColor;

void main() {
    vec2 r = gl_PointCoord*2. - 1.;
    float dist = dot(r, r);
    if (dist > 1.) discard;
    float depth = sqrt(1.0 - dist);
    float thickness = 2.f * depth * particleRadius;
    FragColor = thickness;
}
