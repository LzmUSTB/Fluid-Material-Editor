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
uniform int Shininess;

uniform float near;               
uniform float fov;                
uniform float aspectRatio;    

uniform mat4x4 inv_View;
uniform mat4x4 inv_Projection;

uniform float fresnelScale;             // 0~1

out vec4 FragColor;

float readThickness(vec2 uv) {
    return texture(thicknessMap, clamp(uv, vec2(0.0), vec2(1.0))).x;
}

float repairThickness(vec2 uv, vec2 texelSize, float thickness) {
    if (thickness > 0.006) {
        return thickness;
    }

    float maxThickness = thickness;
    float sum = 0.0;
    float weightSum = 0.0;

    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            vec2 sampleUv = uv + vec2(x, y) * texelSize;
            float sampleThickness = readThickness(sampleUv);
            maxThickness = max(maxThickness, sampleThickness);

            float w = (x == 0 && y == 0) ? 1.0 : 0.5;
            sum += sampleThickness * w;
            weightSum += w;
        }
    }

    float avgThickness = sum / max(weightSum, 1e-6);
    return max(thickness, max(avgThickness, maxThickness * 0.35));
}

vec3 getViewPos(vec2 uv,float depth) {
    float x = uv.x * 2.0 - 1.0;
    float y = -(uv.y * 2.0 - 1.0);
    float tanHalfFov = tan(fov * 0.5);
    return vec3(
        x * depth * tanHalfFov * aspectRatio,
        y * depth * tanHalfFov,
        -depth
    );
}

void main() {
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / textureSize(sceneColor,0);
    vec2 texelSize = 1 / textureSize(sceneColor, 0);
    vec3 bgColor = texture(sceneColor, uv).xyz;
    
    float thickness = texture(thicknessMap, uv).x;

    float depth = texture(depthMap, uv).x;
    vec3 normal = texture(normalMap, uv).xyz;

    if (depth <= 1e-5 || depth > 1e6) {
        FragColor = vec4(bgColor, 1.0);
        return;
    }

    if (dot(normal, normal) <= 1e-8) {
        normal = vec3(0.0, 0.0, 1.0);
    }
    else {
        normal = normalize(normal);
    }

    thickness = repairThickness(uv, texelSize, thickness);
    float surfaceCoverage = smoothstep(0.001, 0.014, thickness);
    float effectiveThickness = max(thickness, 0.012 * surfaceCoverage);

    vec3 viewPos = getViewPos(uv, depth);
    vec3 viewDir = normalize(viewPos);
    vec4 lightViewPos = inverse(inv_View) * vec4(LightPosition,1);
    vec3 lightDir = normalize(lightViewPos.xyz - viewPos);

    // Bling-Phong
    vec3 halfDir = normalize(lightDir - viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfDir), 0.0), Shininess);

    vec3 diffuse = FluidColor * diff;
    vec3 specular = LightColor * spec;

    // refraction
    vec3 refractDir = refract(-viewDir, normal, 0.75);
    vec4 offsetPos = inverse(inv_Projection) * vec4(refractDir,1);
    float droplet = smoothstep(0.002, 0.03, effectiveThickness);
    vec2 offset = normal.xy * refractOffsetAmount * droplet;
    float absorptionStrength = max(absorption, 0.02);
    vec3 transmittance = exp(-effectiveThickness * absorptionStrength * 40.0 * (1.0 - FluidColor)); 
    vec3 refractColor = texture(sceneColor, clamp(uv + offset, vec2(0.0), vec2(1.0))).xyz * transmittance;

    // reflection
    vec3 reflectDir = reflect(viewDir, normal);
    vec3 reflectDirWorld = (inv_View * vec4(reflectDir, 0.0)).xyz;
    vec3 reflectColor = texture(Skybox, reflectDirWorld).xyz;

    // fresnel
    float cosTheta = clamp(dot(normal, -viewDir), 0.0, 1.0);
    float fresnel = fresnelScale + (1.0 - fresnelScale) * pow(1.0 - cosTheta, 5.0);
    fresnel = clamp(fresnel, 0.0, 0.55);

    vec3 reflected = reflectColor * (0.35 + 0.65 * fresnel);
    vec3 finalFluidColor = specular * 0.65 + mix(refractColor, reflected, fresnel);
    float alpha = clamp(1.0 - exp(-effectiveThickness * absorptionStrength * 50.0), 0.0, 1.0);
    alpha = max(alpha, 0.16 * surfaceCoverage);
    alpha = max(alpha, 0.18 * droplet);

    vec3 finalColor = mix(bgColor, finalFluidColor, alpha);
    FragColor = vec4(finalColor,1);
}
