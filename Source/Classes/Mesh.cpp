#include "Mesh.h"
#include <iostream>

Mesh::Mesh(FbxMesh* fbxMesh)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec3> normals, tangents, bitangents;
	
	// Get position array
	for (int i = 0; i < fbxMesh->GetControlPointsCount(); ++i)
	{
		FbxVector4 pos = fbxMesh->GetControlPointAt(i);
		positions.push_back(glm::vec3(pos[0], pos[1], pos[2]));
	}

	// Get uv0 array
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);
	const FbxGeometryElementUV* uv0 = fbxMesh->GetElementUV(uvNames.GetStringAt(0));
	for (int i = 0; i < uv0->GetDirectArray().GetCount(); ++i)
	{
		FbxVector2 uv = uv0->GetDirectArray().GetAt(i);
		textureCoords.push_back(glm::vec2(uv[0], uv[1]));
	}

	// Get normal array
	const FbxLayerElementNormal* fbxNormals = fbxMesh->GetLayer(0)->GetNormals();
	for (int i = 0; i < fbxNormals->GetDirectArray().GetCount(); ++i)
	{
		FbxVector4 fbxNormal = fbxNormals->GetDirectArray().GetAt(i);
		normals.push_back(glm::vec3(fbxNormal[0], fbxNormal[1], fbxNormal[2]));
	}

	// Get tangent array
	const FbxLayerElementTangent* fbxTangents = fbxMesh->GetLayer(0)->GetTangents();
	for (int i = 0; i < fbxTangents->GetDirectArray().GetCount(); ++i)
	{
		FbxVector4 fbxTangent = fbxTangents->GetDirectArray().GetAt(i);
		tangents.push_back(glm::vec3(fbxTangent[0], fbxTangent[1], fbxTangent[2]));
	}

	// Get Bitangent array
	const FbxLayerElementBinormal* fbxBitangents = fbxMesh->GetLayer(0)->GetBinormals();
	for (int i = 0; i < fbxBitangents->GetDirectArray().GetCount(); ++i)
	{
		FbxVector4 fbxBitangent = fbxBitangents->GetDirectArray().GetAt(i);
		bitangents.push_back(glm::vec3(fbxBitangent[0], fbxBitangent[1], fbxBitangent[2]));
	}

	// Get VertexIndex array
	// UV process
	std::vector<VertexIndex> indexArray;
	bool useIndex = uv0->GetReferenceMode() != FbxGeometryElement::eDirect;
	int polyIndexCounter = 0;
	int indexCount = 0;
	for (int i = 0; i < fbxMesh->GetPolygonCount(); ++i)
	{
		for (int j = 0; j < fbxMesh->GetPolygonSize(i); ++j)
		{
			VertexIndex vertexIndex;
			vertexIndex.PosIndex = fbxMesh->GetPolygonVertex(i, j);
			if (uv0->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				vertexIndex.TexIndex = (useIndex) ? uv0->GetIndexArray().GetAt(vertexIndex.PosIndex) : vertexIndex.PosIndex;
			}
			else if (uv0->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				vertexIndex.TexIndex = (useIndex) ? uv0->GetIndexArray().GetAt(polyIndexCounter) : polyIndexCounter;
				polyIndexCounter++;
			}
			indexArray.push_back(vertexIndex);
			this->Indices.push_back(indexCount++);
		}
	}

	// Normal process
	if (fbxNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		for (int i = 0; i < indexArray.size(); ++i)
		{
			indexArray[i].NorIndex = indexArray[i].PosIndex;
		}
	}
	else if (fbxNormals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int indexByPolyVertex = 0;
		for (int i = 0; i < fbxMesh->GetPolygonCount(); ++i)
		{
			for (int j = 0; j < fbxMesh->GetPolygonSize(i); ++j)
			{
				if (fbxNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
					indexArray[indexByPolyVertex].NorIndex = indexByPolyVertex;
				if (fbxNormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					indexArray[indexByPolyVertex].NorIndex = fbxNormals->GetIndexArray().GetAt(indexByPolyVertex);
				indexByPolyVertex++;
			}
		}
	}

	// Tangent process
	if (fbxTangents->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		for (int i = 0; i < indexArray.size(); ++i)
		{
			indexArray[i].TanIndex = indexArray[i].PosIndex;
		}
	}
	else if (fbxTangents->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int indexByPolyVertex = 0;
		for (int i = 0; i < fbxMesh->GetPolygonCount(); ++i)
		{
			for (int j = 0; j < fbxMesh->GetPolygonSize(i); ++j)
			{
				if (fbxTangents->GetReferenceMode() == FbxGeometryElement::eDirect)
					indexArray[indexByPolyVertex].TanIndex = indexByPolyVertex;
				if (fbxTangents->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					indexArray[indexByPolyVertex].TanIndex = fbxTangents->GetIndexArray().GetAt(indexByPolyVertex);
				indexByPolyVertex++;
			}
		}
	}

	// Bitangent process
	if (fbxBitangents->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		for (int i = 0; i < indexArray.size(); ++i)
		{
			indexArray[i].BitIndex = indexArray[i].PosIndex;
		}
	}
	else if (fbxBitangents->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int indexByPolyVertex = 0;
		for (int i = 0; i < fbxMesh->GetPolygonCount(); ++i)
		{
			for (int j = 0; j < fbxMesh->GetPolygonSize(i); ++j)
			{
				if (fbxBitangents->GetReferenceMode() == FbxGeometryElement::eDirect)
					indexArray[indexByPolyVertex].BitIndex = indexByPolyVertex;
				if (fbxBitangents->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					indexArray[indexByPolyVertex].BitIndex = fbxBitangents->GetIndexArray().GetAt(indexByPolyVertex);
				indexByPolyVertex++;
			}
		}
	}

	bool DropMethod = fbxNormals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex && fbxNormals->GetReferenceMode() == FbxGeometryElement::eDirect;
	std::vector<bool> SamePosUVMarker;
//  	if (!DropMethod)
//  		DropRepeatVertexIndex(indexArray);
//  	else
//  		MarkSamePosUV(indexArray, SamePosUVMarker);

	// Build new Vertices array
	for (int i = 0; i < indexArray.size(); ++i)
	{
		Vertex newVertex;
		newVertex.Position = positions[indexArray[i].PosIndex];
		newVertex.TextureCoord = textureCoords[indexArray[i].TexIndex];
		newVertex.Normal = normals[indexArray[i].NorIndex];
		newVertex.Tangent = tangents[indexArray[i].TanIndex];
		newVertex.Bitangent = bitangents[indexArray[i].BitIndex];
		this->Vertices.push_back(newVertex);
	}

//  	if (DropMethod)
//  		DropRepeatNTB(SamePosUVMarker);

	// Get material indices map
	std::vector<std::string> matNames;
	for (int i = 0; i < fbxMesh->GetNode()->GetMaterialCount(); ++i)
	{
		matNames.push_back(fbxMesh->GetNode()->GetMaterial(i)->GetName());
		SplitIndices.insert(std::pair<std::string, std::vector<unsigned int>>(matNames[i], {}));
	}
	FbxGeometryElementMaterial* fbxMatObj = fbxMesh->GetElementMaterial();
	for (int i = 0; i < fbxMatObj->GetIndexArray().GetCount(); ++i)
	{
		int nameIndex = fbxMatObj->GetIndexArray().GetAt(i);
		SplitIndices[matNames[nameIndex]].push_back(Indices[i * 3]);
		SplitIndices[matNames[nameIndex]].push_back(Indices[i * 3 + 1]);
		SplitIndices[matNames[nameIndex]].push_back(Indices[i * 3 + 2]);
	}
}

void Mesh::DropRepeatVertexIndex(std::vector<VertexIndex>& sourceArray)
{
	// Need refresh indices
	this->Indices.clear();

	std::vector<VertexIndex> optimizedIndexArray;
	for (int i = 0; i < sourceArray.size(); ++i)
	{
		int newIndex = -1;
		for (int k = 0; k < optimizedIndexArray.size(); ++k)
		{
			if (sourceArray[i] == optimizedIndexArray[k])
			{
				newIndex = k;
				break;
			}
		}
		if (newIndex == -1)
		{
			newIndex = optimizedIndexArray.size();
			optimizedIndexArray.push_back(sourceArray[i]);
		}
		this->Indices.push_back(newIndex);
	}
	sourceArray.clear();
	sourceArray.assign(optimizedIndexArray.begin(), optimizedIndexArray.end());
}

void Mesh::MarkSamePosUV(const std::vector<VertexIndex>& sourceArray, std::vector<bool>& SamePosUVMarker)
{
	std::vector<VertexIndex> optimizedIndexArray;
	for (int i = 0; i < sourceArray.size(); ++i)
	{
		int newIndex = -1;
		for (int k = 0; k < optimizedIndexArray.size(); ++k)
		{
			if (sourceArray[i].PartCompare(optimizedIndexArray[k]))
			{
				newIndex = k;
				break;
			}
		}
		if (newIndex == -1)
		{
			newIndex = optimizedIndexArray.size();
			optimizedIndexArray.push_back(sourceArray[i]);
			SamePosUVMarker.push_back(false);
		}
		else
			SamePosUVMarker.push_back(true);
	}
}

void Mesh::DropRepeatNTB(const std::vector<bool>& SamePosUVMarker)
{
	std::vector<Vertex> optizedVertexArray;
	for (int i = 0; i < this->Indices.size(); ++i)
	{
		int newIndex = -1;
		if (SamePosUVMarker[i])
		{
			for (int j = 0; j < optizedVertexArray.size(); ++j)
			{
				if (this->Vertices[i].Normal == optizedVertexArray[j].Normal && this->Vertices[i].Tangent == optizedVertexArray[j].Tangent)
				{
					newIndex = j;
					break;
				}
			}
		}
		if (newIndex == -1)
		{
			newIndex = optizedVertexArray.size();
			optizedVertexArray.push_back(this->Vertices[i]);
		}
		else
		{
			this->Indices[i] = newIndex;
		}
	}
	this->Vertices.clear();
	this->Vertices.assign(optizedVertexArray.begin(), optizedVertexArray.end());
}

void Mesh::DebugOut()
{
	for (int i = 0; i < this->Vertices.size(); ++i)
 	{
 		std::cout << this->Vertices[i].Position.r << "," << this->Vertices[i].Position.g << "," << this->Vertices[i].Position.b << ",";
 	}
 	std::cout << std::endl;
 	for (int i = 0; this->Indices.size(); ++i)
 	{
 		std::cout << this->Indices[i] << ",";
 	}
}