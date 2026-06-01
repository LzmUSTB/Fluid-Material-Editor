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
    if (currentValue <= 1e-5 || currentValue > 1e6) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    float sigma = max(float(blurSize), 1.0) / 3.0;
    float sigma2 = 2. * sigma * sigma;
    float sigmaDepth = clamp(threshold + offsetFix * 0.001, 0.012, 0.075);
    float sigmaDepth2 = 2.0 * sigmaDepth * sigmaDepth;
    float hardDepthRange = sigmaDepth * 3.0;

    float result = currentValue;
    float weight_sum = GaussianBlurWeight(ivec2(0,0),sigma2);

    for(int i = 1; i <= blurSize; ++i) {
        float value = texture(SampleTexture, uv + direction * i).r;
        float value_i = texture(SampleTexture, uv - direction * i).r;

        ivec2 uvOffset = Horizontal ? ivec2(i,0) : ivec2(0,i);
        float weight = GaussianBlurWeight(uvOffset,sigma2);

        if (value > 1e-5 && value < 1e6) {
            float dz = value - currentValue;
            if (abs(dz) > hardDepthRange) {
                dz = 1e6;
            }
            float depthWeight = exp(-(dz * dz) / sigmaDepth2);
            result += value * weight * depthWeight;
            weight_sum += weight * depthWeight;
        }

        if (value_i > 1e-5 && value_i < 1e6) {
            float dz_i = value_i - currentValue;
            if (abs(dz_i) > hardDepthRange) {
                dz_i = 1e6;
            }
            float depthWeight_i = exp(-(dz_i * dz_i) / sigmaDepth2);
            result += value_i * weight * depthWeight_i;
            weight_sum += weight * depthWeight_i;
        }
    }

    result /= weight_sum;
    FragColor = vec4(result,result,result, 1.0);
}
