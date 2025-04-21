#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform mat4 Model_i;
uniform vec3 CameraPos;

out vec3 fragPos;

void main(){
	vec3 center = vec3(0, 0, 0);
	vec4 viewer = Model_i * vec4(CameraPos,1.0f);
	vec3 normalDir = viewer.xyz - center;

	normalDir = normalize(normalDir);
	vec3 upDir = abs(normalDir.y) > 0.999 ? vec3(0, 0, 1) : vec3(0, 1, 0);

	vec3 rightDir = normalize(cross(upDir, normalDir));
	upDir = normalize(cross(normalDir, rightDir));

	vec3 centerOffs = aPos - center;
	vec3 localPos = center + rightDir * centerOffs.x + upDir * centerOffs.y + normalDir * centerOffs.z;
	
	gl_Position = Projection * View * Model * vec4(localPos, 1.0f); 
	fragPos = aPos;
}