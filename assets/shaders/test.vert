#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}