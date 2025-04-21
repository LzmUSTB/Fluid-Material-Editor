#version 460 core
uniform sampler2D ThicknessMap;

out vec4 FragColor;


void main(){
    ivec2 texSize = textureSize(ThicknessMap, 0);
    vec2 uv = (floor(gl_FragCoord.xy) + vec2(0.5)) / vec2(texSize);
    vec2 pixel = 1.0 / vec2(texSize);

    float center = texture(ThicknessMap, uv).x;
    float left   = texture(ThicknessMap, uv - vec2(pixel.x, 0)).x;
    float right  = texture(ThicknessMap, uv + vec2(pixel.x, 0)).x;
    float top    = texture(ThicknessMap, uv + vec2(0, pixel.y)).x;
    float bottom = texture(ThicknessMap, uv - vec2(0, pixel.y)).x;

    float scale = 50.0;
    vec3 dx = vec3(2.0 * pixel.x, 0.0, (right - left) * scale);
    vec3 dy = vec3(0.0, 2.0 * pixel.y, (top - bottom) * scale);
    vec3 normal = normalize(cross(dx, dy));

    FragColor = vec4(normal, 1.0); 
}