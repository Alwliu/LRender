#pragma once
#include "glm/glm.hpp"
#include "fbxsdk.h"
#include <vector>
#include <map>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TextureCoord;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct VertexIndex
{
	unsigned int PosIndex = 0;
	unsigned int TexIndex = 0;
	unsigned int NorIndex = 0;
	unsigned int TanIndex = 0;
	unsigned int BitIndex = 0;

	inline bool operator == (const VertexIndex& target) const
	{
		return this->PosIndex == target.PosIndex && this->TexIndex == target.TexIndex && this->NorIndex == target.NorIndex && this->TanIndex == target.TanIndex && this->BitIndex == target.BitIndex;
	}
	inline bool PartCompare(const VertexIndex& target) const
	{
		return this->PosIndex == target.PosIndex && this->TexIndex == target.TexIndex;
	}
};

class Mesh
{
public:
	std::vector<Vertex> Vertices; // Vertices data
	std::vector<unsigned int> Indices; // All Indices
	std::map<std::string, std::vector<unsigned int>> SplitIndices; // Split by material indices, material id : indices

	unsigned int VBO, VAO, EBO;

	// Constructor
	Mesh() {};
	Mesh(FbxMesh* fbxMesh);

private:
	// Remove repeat item
	void DropRepeatVertexIndex(std::vector<VertexIndex>& sourceArray);

	// Remove repeat normal, tangent item
	void MarkSamePosUV(const std::vector<VertexIndex>& sourceArray, std::vector<bool>& SamePosUVMarker);
	void DropRepeatNTB(const std::vector<bool>& SamePosUVMarker);

public:
	void DebugOut();
};