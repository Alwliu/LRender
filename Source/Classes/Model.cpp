#include "Model.h"

Model::Model(FbxScene* scene)
{
	FbxNode* rootNode = scene->GetRootNode();
	std::vector<FbxMesh*> fbxMeshes;
	GetFbxMesh(rootNode, fbxMeshes);
	for (int i = 0; i < fbxMeshes.size(); ++i)
	{
		Mesh mesh(fbxMeshes[i]);
		this->Meshes.push_back(mesh);
	}

	for (int i = 0; i < scene->GetMaterialCount(); ++i)
	{
		FbxSurfaceMaterial* material = scene->GetMaterial(i);
		Material mat;
		mat.ID = i;
		mat.Name = material->GetName();
		this->Materials.push_back(mat);
	}
}

void Model::GetFbxMesh(FbxNode* node, std::vector<FbxMesh*>& fbxMeshes)
{
	if (node->GetChildCount() == 0)
	{
		if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			fbxMeshes.push_back(node->GetMesh());
		}
		return;
	}
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		FbxNode* childNode = node->GetChild(i);
		GetFbxMesh(childNode, fbxMeshes);
	}
}
