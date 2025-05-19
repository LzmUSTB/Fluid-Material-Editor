#version 460 core
layout(binding = 0) uniform sampler2D SampleTexture;
uniform bool Horizontal;
uniform float blurSize;

const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 FragColor;

void main() {
    vec2 texSize = textureSize(SampleTexture, 0);
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(SampleTexture, 0);
    vec2 direction = Horizontal ? vec2(1/texSize.x, 0) : vec2(0, 1/texSize.y);
    direction = direction * blurSize;
    vec3 result = texture(SampleTexture, uv).rgb * weight[0];
    
    for (int i = 1; i < 5; ++i) {
        result += texture(SampleTexture, uv + direction * i).rgb * weight[i];
        result += texture(SampleTexture, uv - direction * i).rgb * weight[i];
    }
    
    FragColor = vec4(result, 1.0);
}