#pragma once
#include "Mesh.h"
#include "Material.h"

class Model
{
public:
	std::vector<Mesh> Meshes;
	std::vector<Material> Materials;

	Model() {};
	Model(FbxScene* scene);

private:
	void GetFbxMesh(FbxNode* node, std::vector<FbxMesh*>& fbxMeshes);
};