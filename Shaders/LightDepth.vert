#version 330 core
layout (location = 0) in vec3 Position;

uniform mat4 LightSpaceMatrix;

void main()
{
	gl_Position = LightSpaceMatrix * vec4(Position, 1.0f);
}