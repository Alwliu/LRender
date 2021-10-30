#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

layout (std140) uniform Matrices
{
	mat4 Projection;
	mat4 View;
};

uniform mat4 LightSpaceMatrix;

out vert {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
} vOut;

void main()
{
	vOut.Normal = Normal;
	vOut.FragPos = Position;
	vOut.FragPosLightSpace = LightSpaceMatrix * vec4(vOut.FragPos, 1.0f);
	gl_Position = Projection * View * vec4(Position, 1.0);
}