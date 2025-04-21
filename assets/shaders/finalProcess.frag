#version 460 core

layout(binding = 0) uniform sampler2D sceneColor; 
layout(binding = 1) uniform sampler2D thicknessMap;
layout(binding = 2) uniform sampler2D normalMap;

uniform float absorption;
uniform float refractOffsetAmount;

out vec4 FragColor;

vec3 applyRefraction(vec2 uv, vec3 normal, float thickness) {
    vec2 refractOffset = normal.xy * thickness * refractOffsetAmount; 
    vec3 bgColor = texture(sceneColor, uv + refractOffset).rgb;
    return bgColor;
}

void main() {
    vec2 uv = gl_FragCoord.xy / textureSize(sceneColor,0);
    
    float thickness = texture(thicknessMap, uv).x;
    
    vec3 normal = texture(normalMap, uv).xyz;
    vec3 refractColor = applyRefraction(uv, normal, thickness);
    
    float alpha = 1.0 - exp(-thickness * absorption);
    vec3 finalColor = mix(refractColor, vec3(0.2, 0.3, 1.0),alpha);
    //vec3 finalColor = mix(texture(sceneColor, uv).rgb, fluidColor, alpha);

    FragColor = vec4(finalColor,1);
}