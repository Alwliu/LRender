#version 330 core
layout (location = 0) in vec3 Position;

layout (std140) uniform Matrices
{
	mat4 Projection;
	mat4 View;
};

void main()
{
	gl_Position = Projection * View * vec4(Position, 1.0);
}