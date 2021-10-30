#include "Light.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 DirLight::GetLightProjection()
{
	return glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, this->NearPlane, this->FarPlane);
}

glm::mat4 DirLight::GetLightView()
{
	return glm::lookAt(this->Position, this->Direction * glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 DirLight::GetLightSpaceMatrix()
{
	return GetLightProjection() * GetLightView();
}
