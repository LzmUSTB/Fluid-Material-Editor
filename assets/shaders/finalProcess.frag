#version 460 core

layout(binding = 0) uniform sampler2D sceneColor; 
layout(binding = 1) uniform sampler2D thicknessMap;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D depthMap;
layout(binding = 4) uniform samplerCube Skybox;

uniform float absorption;
uniform float refractOffsetAmount;
uniform vec3 FluidColor;
uniform vec3 LightColor;
uniform vec3 LightPosition;             // world space position

uniform float near;               
uniform float fov;                
uniform float aspectRatio;    

uniform mat4x4 inv_View;
uniform mat4x4 inv_Projection;

uniform float fresnelScale;             // 0~1

out vec4 FragColor;

vec3 applyRefraction(vec2 uv, vec3 normal, float thickness) {
    vec2 refractOffset = normal.xy * thickness * refractOffsetAmount; 
    vec3 bgColor = texture(sceneColor, uv + refractOffset).rgb;
    return bgColor;
}

vec3 getViewPos(vec2 uv,float depth) {
    float x = (uv.x * 2.0 - 1.0);
    float y = -(uv.y * 2.0 - 1.0);

    //float tanHalfFov = tan(fov * 0.5);
    //float viewX = x * z * tanHalfFov * aspectRatio;
    //loat viewY = y * z * tanHalfFov;
    vec4 ndc = vec4(x,y,0,1);
    ndc.z = -inverse(inv_Projection)[2].z + inverse(inv_Projection)[3].z / depth;
    vec4 viewPos = inv_Projection * ndc;
    return viewPos.xyz / viewPos.w; 
}

void main() {
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(sceneColor,0);
    vec2 texelSize = 1 / textureSize(sceneColor, 0);
    vec3 bgColor = texture(sceneColor, uv).xyz;
    
    float thickness = texture(thicknessMap, uv).x;

    float depth = texture(depthMap, uv).x;
    vec3 normal = texture(normalMap, uv).xyz;

    vec3 viewPos = getViewPos(uv, depth);
    vec3 viewDir = normalize(viewPos);
    vec4 lightViewPos = inverse(inv_View) * vec4(LightPosition,1);
    vec3 lightDir = normalize(lightViewPos.xyz - viewPos);

    // Bling-Phong
    vec3 halfDir = normalize(lightDir - viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfDir), 0.0), 250.0);

    vec3 diffuse = FluidColor * diff;
    vec3 specular = LightColor * spec;

    vec3 refractDir = refract(-viewDir, normal, 0.75);
    vec4 offsetPos = inverse(inv_Projection) * vec4(refractDir,1);
    vec2 offset = normal.xy * texelSize;
    //vec2 offset = (offsetPos.xyz/offsetPos.w).xy;
    vec3 transmittance = exp(-thickness * (1-FluidColor)); 
    vec3 refractColor = texture(sceneColor, uv+offset).xyz * transmittance;
    //vec3 refractColor = applyRefraction(uv, normal, thickness);

    //float alpha = 1.0 - exp(-thickness * absorption);
    //refractColor = mix(refractColor, diffuse, alpha);

    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 reflectDirWorld = (inv_View * vec4(reflectDir, 0.0)).xyz;
    vec3 reflectColor = texture(Skybox,reflectDirWorld).xyz;

    // fresnel
    float fresnel = fresnelScale + (1-fresnelScale) * pow(1.0 - max(dot(normal, -viewDir),0.), 5.0);

    vec3 finalFluidColor = specular + mix(refractColor, reflectColor, clamp(fresnel,0,1));
    float alpha = 1.0 - exp(-thickness);

    vec3 finalColor = mix(bgColor, finalFluidColor, alpha);
    FragColor = vec4(finalColor,1);
}