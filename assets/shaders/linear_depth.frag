#version 460 core
uniform sampler2D Depthmap;
uniform float Near; 
uniform float Far;  

out vec4 FragColor;

void main() {
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(Depthmap, 0);
    float depth = texture(Depthmap,uv).r;
    //float linearDepth = (depth - Near) / (Far - Near);
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0*Near*Far)/(Far+Near-z*(Far-Near)); 
    FragColor = vec4(linearDepth, 0, 0, 1.0);
    //FragColor = vec4(depth,0,0,1);
}