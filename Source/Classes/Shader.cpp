#include "Shader.h"
#include <iostream>

Shader& Shader::Use()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::Compile(const GLchar* vertSource, const GLchar* fragmentSource, const GLchar* geoSource)
{
	GLuint sVert, sFragment, gShader;
	// Vertex shader
	sVert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVert, 1, &vertSource, NULL);
	glCompileShader(sVert);
	CheckCompileErrors(sVert, CompileType::VERTEX);
	// Fragment shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	CheckCompileErrors(sFragment, CompileType::FRAGMENT);
	// Geometry shader
	if (nullptr != geoSource)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geoSource, NULL);
		glCompileShader(gShader);
		CheckCompileErrors(gShader, CompileType::GEOMETRY);
	}
	// Shader program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVert);
	glAttachShader(this->ID, sFragment);
	if (nullptr != geoSource)
		glAttachShader(this->ID, gShader);
	glLinkProgram(this->ID);
	CheckCompileErrors(this->ID, CompileType::PROGRAM);

	// Delete shaders
	glDeleteShader(sVert);
	glDeleteShader(sFragment);
	if (nullptr != geoSource)
		glDeleteShader(gShader);
}

void Shader::CheckCompileErrors(GLuint obj, CompileType type)
{
	GLint sucess;
	GLchar infoLog[1024];
	if (type != CompileType::PROGRAM)
	{
		glGetShaderiv(obj, GL_COMPILE_STATUS, &sucess);
		if (!sucess)
		{
			glGetShaderInfoLog(obj, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << (char*)type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(obj, GL_LINK_STATUS, &sucess);
		if (!sucess)
		{
			glGetProgramInfoLog(obj, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << "PROGRAM" << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, unsigned int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
