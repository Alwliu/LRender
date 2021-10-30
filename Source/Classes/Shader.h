#pragma once
#include <string>
#include <glm/glm.hpp>
#include "glad/glad.h"

enum class CompileType
{
	PROGRAM,
	VERTEX,
	FRAGMENT,
	GEOMETRY
};

class Shader
{
public:
	GLuint ID;

	Shader& Use();
	void Compile(const GLchar* vertSource, const GLchar* fragmentSource, const GLchar* geoSource = nullptr);

private:
	void CheckCompileErrors(GLuint obj, CompileType type);

public:
	void setFloat(const std::string& name, float value) const;
	void setInt(const std::string& name, unsigned int value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
};