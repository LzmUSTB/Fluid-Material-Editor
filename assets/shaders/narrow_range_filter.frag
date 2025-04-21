#version 460 core

layout(binding = 0) uniform sampler2D SampleTexture;
uniform float NarrowFilterRange;
uniform float Offset;
uniform bool Horizontal;

const float Gweight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 FragColor;

void main() {
    vec2 texSize = textureSize(SampleTexture, 0);
    vec2 uv = gl_FragCoord.xy / texSize;
    float result = 0;
    float weight_sum = 0;
    vec2 direction = Horizontal ? vec2(1/texSize.x, 0) : vec2(0, 1/texSize.y);
    float current = texture(SampleTexture, uv ).r;
    
    for(int i=1; i<5; i++) {
        float value = texture(SampleTexture, uv + direction * i).r;
        float value_i = texture(SampleTexture, uv - direction * i).r;
        if(value>current+NarrowFilterRange || value_i>current+NarrowFilterRange){
            continue;
        }

        value = (value>current-NarrowFilterRange) ? value : current-Offset;
        result += value * Gweight[i];
        result += value_i * Gweight[i];
    }

    FragColor = vec4(result,result,result, 1.0);
}