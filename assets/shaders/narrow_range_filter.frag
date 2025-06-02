#version 460 core

layout(binding = 0) uniform sampler2D SampleTexture;
uniform float threshold;
uniform float offsetFix;
uniform bool Horizontal;
uniform int blurSize;

out vec4 FragColor;

float GaussianBlurWeight(ivec2 uv, float sigma2){
    int sqrDst = uv.x * uv.x + uv.y * uv.y;
    return exp(-sqrDst / sigma2);
}

void main() {
    vec2 texSize = textureSize(SampleTexture, 0);
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(SampleTexture, 0);
    vec2 direction = Horizontal ? vec2(1/texSize.x, 0) : vec2(0, 1/texSize.y);
    float currentValue = texture(SampleTexture, uv).r;

    float sigma = blurSize / 3.0;
    float sigma2 = 2. * sigma * sigma;

    float result = currentValue;
    float weight_sum = GaussianBlurWeight(ivec2(0,0),sigma2);

    for(int i = 1; i <= blurSize; ++i) {
        float value = texture(SampleTexture, uv + direction * i).r;
        float value_i = texture(SampleTexture, uv - direction * i).r;

        float dz = abs(value - currentValue);
        float dz_i = abs(value_i - currentValue);

        float dynamicThreshold = threshold * (dz + dz_i) * 0.5 + 0.001;

        if(dz > dynamicThreshold || dz_i > dynamicThreshold){
            continue;
        }

        ivec2 uvOffset = Horizontal ? ivec2(i,0) : ivec2(0,i);
        float weight = GaussianBlurWeight(uvOffset,sigma2);

        result += value * weight;
        result += value_i * weight;
        weight_sum += weight * 2;
    }

    result /= weight_sum;
    FragColor = vec4(result,result,result, 1.0);
}