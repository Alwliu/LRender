#pragma once
#include "glm/glm.hpp"
#include <string>

class DirLight
{
public:
	glm::vec4 Color;
	glm::vec3 Position;
	glm::vec3 Direction;
	bool bShadow;

	std::string name;

	float NearPlane, FarPlane;

	DirLight() : Color(glm::vec4(1.0f)), Position(glm::vec3(0.0f)), Direction(glm::vec3(1.0f, 0.0f, 0.0f)), bShadow(true), NearPlane(1.0f), FarPlane(7.5f), name("") {};

	glm::mat4 GetLightProjection();
	glm::mat4 GetLightView();
	glm::mat4 GetLightSpaceMatrix();
};