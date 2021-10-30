#include "ResourceManager.h"
#include "Butility/stb_image.h"
#include <fstream>
#include <sstream>

std::map<std::string, Model> ResourceManager::Models;
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Material> ResourceManager::Materials;
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, DirLight> ResourceManager::Lighters;
unsigned int ResourceManager::MeshCount(0);

bool ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	std::string vertCode, fragCode, geoCode;
	// Open files
	std::ifstream vertShaderFile(vShaderFile);
	std::ifstream fragShaderFile(fShaderFile);
	std::stringstream vShderStream, fShaderStream;
	// Read file's buffer contents into streams
	vShderStream << vertShaderFile.rdbuf();
	fShaderStream << fragShaderFile.rdbuf();
	// Close file handlers
	vertShaderFile.close();
	fragShaderFile.close();
	// Convert stream into string
	vertCode = vShderStream.str();
	fragCode = fShaderStream.str();
	// If geometry shader path is present, also load a geometry shader
	if (nullptr != gShaderFile)
	{
		std::ifstream geoShaderFile(gShaderFile);
		std::stringstream gShaderStream;
		gShaderStream << geoShaderFile.rdbuf();
		geoShaderFile.close();
		geoCode = gShaderStream.str();
	}

	const char* vShaderCode = vertCode.c_str();
	const char* fShaderCode = fragCode.c_str();
	const char* gShaderCode = geoCode.c_str();

	Shader shader;
	std::string shaderName = GetFileName(vShaderFile);
	shader.Compile(vShaderCode, fShaderCode, nullptr != gShaderFile ? gShaderCode : nullptr);
	Shaders[shaderName] = shader;
	return true;
}

bool ResourceManager::loadModelFromFile(const char* fbxFile, bool triangulate)
{
	FbxManager* FbxImportManager = FbxManager::Create();
	FbxScene* scene = FbxScene::Create(FbxImportManager, "");
	FbxIOSettings* ios = FbxIOSettings::Create(FbxImportManager, IOSROOT);
	FbxImportManager->SetIOSettings(ios);
	FbxImporter* importer = FbxImporter::Create(FbxImportManager, "");

	std::string modelName = GetFileName(fbxFile);

	if (!importer->Initialize(fbxFile, -1, FbxImportManager->GetIOSettings()))
	{
		FbxImportManager->Destroy();
		return false;
	}
	else
	{
		importer->Import(scene);
		if (triangulate)
		{
			FbxGeometryConverter geoConverter(FbxImportManager);
			geoConverter.Triangulate(scene, true);
		}
		Model model(scene);
		Models[modelName] = model;
		MeshCount += model.Meshes.size();

		FbxImportManager->Destroy();
		return true;
	}
}

bool ResourceManager::loadTextureFromFile(const char* textureFile, GLboolean alpha)
{
	std::string textureName = GetFileName(textureFile);
	Texture2D texture;
	// Load image
	int width, height, nrChannels;
	unsigned char* image = stbi_load(textureFile, &width, &height, &nrChannels, 0);
	if (nrChannels > 3)
	{
		texture.Image_Format = GL_RGBA;
		if (alpha)
		{
			texture.Internal_Format = GL_RGBA;
		}
	}
	if (image)
	{
		texture.Generate(width, height, image);
		stbi_image_free(image);

		Textures[textureName] = texture;
		return true;
	}
	else
		return false;	
}

 void ResourceManager::CreateDirectionalLight(const char* name)
 {
 	DirLight light;
	light.name = std::string(name);
 	ResourceManager::Lighters[light.name] = light;
 }

void ResourceManager::clear()
{
	Shaders.clear();
	Models.clear();
	Materials.clear();
	Textures.clear();
	Lighters.clear();
}

std::string ResourceManager::GetFileName(const char* filePath)
{
	std::string path = std::string(filePath);
	unsigned int nameStart = path.find_last_of("/") + 1;
	unsigned int nameEnd = path.find_last_of(".");
	std::string name = path.substr(nameStart, nameEnd - nameStart);
	return name;
}
