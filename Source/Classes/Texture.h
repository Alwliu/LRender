#pragma once
#include "glad/glad.h"

class Texture2D
{
public:
	// Texture object id
	GLuint ID;
	// Texture size
	GLuint Width, Height;
	// Texture format
	GLuint Internal_Format; // Format of texture object
	GLuint Image_Format; // Format of loaded image
	// Texture configuration
	GLuint Wrap_S; // Wrapping mode on S axis
	GLuint Wrap_T; // Wrapping mode on T axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels
	// Constructor
	Texture2D();
	Texture2D(GLuint width, GLuint height);
	// Generate texture from image data
	void Generate(GLuint width, GLuint height, unsigned char* data);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
};