#include "SkeletonMesh.h"

SkeletonMesh::SkeletonMesh(Graphics& gfx, const std::string fileName, float scale)
{
	//get and save file patch
	std::size_t pos = fileName.find_last_of('\\');
	if (pos != std::string::npos) {
		filePatch = fileName.substr(0, pos + 1);
	}

	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

void SkeletonMesh::Draw(Graphics& gfx) const
{
}

std::unique_ptr<Mesh> SkeletonMesh::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{

	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

	Dvtx::VertexLayout layout;
	layout
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bitangent)
		.Append(VertexLayout::Texture2D)
	;

	Dvtx::VertexBuffer vbuf(std::move(layout));

	std::vector<std::vector<float>> bonesWeight(mesh.mNumVertices, std::vector<float>(mesh.mNumBones, 0.0f));
	bonesWeight.reserve(mesh.mNumVertices);
	for (int i = 0; i < mesh.mNumBones; i++) 
	{
		layout.Append(VertexLayout::BoneWight);
		auto a = mesh.mBones[i];
		for (int j = 0; j < mesh.mBones[i]->mNumWeights; j++)
		{
			auto b = a->mWeights[j];
			bonesWeight[b.mVertexId][i] = b.mWeight;
		}
	}

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBackSpread(bonesWeight[i], 0,
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return std::unique_ptr<Mesh>();
}

std::unique_ptr<Node> SkeletonMesh::ParseNode(int& nextId, const aiNode& node) noexcept
{
	return std::unique_ptr<Node>();
}

SkMesh::SkMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs):
	Mesh(gfx,std::move(bindPtrs))
{
}
