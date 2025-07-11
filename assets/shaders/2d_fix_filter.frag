#version 460 core
uniform sampler2D SampleTexture;
uniform float blurSize;
out vec4 FragColor;

void main(){
    vec2 texSize = textureSize(SampleTexture, 0);
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(SampleTexture, 0);

    vec3 result = vec3(0);
    vec2 direction = vec2(1/texSize.x,1/texSize.y);
    
    for(int i=-1;i<2;i++){
        for(int j=-1;j<2;j++){
        result += texture(SampleTexture, uv + direction*vec2(i,j)).rgb;
        }
    }
    
    FragColor = vec4(result/9., 1.0);
}