#version 460 core

in vec3 fragPos;
out vec4 FragColor;

void main(){
	float depth = gl_FragCoord.z;
	float normalizedZ = (fragPos.z + 1.0) * 0.5;
	FragColor = vec4(fragPos*0.5+0.5, 1.0);
}