#pragma once
#include <vector>
#include "Model.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"
#include "Shader.h"
#include <map>

enum class ShadingModel
{
	Unlit,
	BlinnPhong,
	LightDepth,
	DebugLightDepth
};

class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Model> Models;
	static std::map<std::string, Material> Materials;
	static std::map<std::string, DirLight> Lighters;
	static std::map<std::string, Texture2D> Textures;
	static unsigned int MeshCount;

	static bool loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile);
	static bool loadModelFromFile(const char* fbxFile, bool triangulate = false);
	static bool loadTextureFromFile(const char* textureFile, GLboolean alpha = false);

	static void CreateDirectionalLight(const char* name);

	static void clear();

private:
	ResourceManager() {};
	static std::string GetFileName(const char* filePath);
};