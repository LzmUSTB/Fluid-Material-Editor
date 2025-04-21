#version 460 core

out vec4 FragColor;
in vec3 fragPos;

void main(){
	FragColor = vec4( fragPos*0.5+0.5, 1.0f );
}