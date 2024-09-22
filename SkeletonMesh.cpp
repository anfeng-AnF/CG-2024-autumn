#include "SkeletonMesh.h"
#include "Transform.h"
#include "DebugGraphsMannger.h"

namespace dx = DirectX;
DirectX::XMMATRIX AiMatrixToXMMATRIX(const aiMatrix4x4& aiMat) {
	DirectX::XMMATRIX xmMat = DirectX::XMMATRIX(
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	);
	return DirectX::XMMatrixTranspose(xmMat);
}

SkeletonMesh::SkeletonMesh(Graphics& gfx, const std::string fileName)
	:
	VCbufBones(gfx,1u)
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
	ParseBone(pRoot.get(), dx::XMMatrixIdentity());
}

void SkeletonMesh::Draw(Graphics& gfx)
{
	this->Bind(gfx);
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void SkeletonMesh::Bind(Graphics& gfx)
{
	assert(bonesName.size() <= 1024 && "Bone count over 1024");
	for (int i = 0; i < bonesName.size(); i++) {
		data->bones[i] =bones[bonesName[i]].FinalTransformation;
		//auto pos = FTransform(data->bones[i]).position;
		//XMFLOAT3 posi;
		//DirectX::XMStoreFloat3(&posi, pos);
		//DebugGraphsMannger::GetInstence().AddGeo(std::make_shared<DebugSphere>(gfx, DirectX::XMFLOAT3{ 0,0,0 },posi));
	}
	VCbufBones.Update(gfx, *data.get());
	VCbufBones.Bind(gfx);
}

std::unique_ptr<SKMesh> SkeletonMesh::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

	//vertexId<bones<pair<boneid,weight>>>
	std::vector<std::vector<std::pair<int, float>>> boneWeights(mesh.mNumVertices);

	assert(!(bonesName.size() != 0 && bonesName.size() != mesh.mNumBones)&&"Using multiple skeletons for one model is not supported at this time");
	bonesName.resize(mesh.mNumBones);
	auto bones = mesh.mBones;
	for (UINT i = 0; i < mesh.mNumBones; i++) {

		//process bone name
		bonesName[i] = (bones[i]->mName.C_Str());
		assert(!(bonesName.size() != 0 && bonesName[i] != bones[i]->mName.C_Str()) && "Using multiple skeletons for one model is not supported at this time");

		BoneInfo boneInfo;
		boneInfo.BoneOffset = AiMatrixToXMMATRIX(bones[i]->mOffsetMatrix);
		this->bones[std::string(bones[i]->mName.C_Str())] = boneInfo;

		auto weights = bones[i]->mWeights;
		for (UINT j = 0; j < bones[i]->mNumWeights; j++) {
			boneWeights[weights[j].mVertexId].push_back({ i, weights[j].mWeight });
		}
	}
	UINT maxBoneAffectVertex = 0;
	for (const auto& v : boneWeights) {
		maxBoneAffectVertex = max(maxBoneAffectVertex, v.size());
	}
	int numBoneX4 = (maxBoneAffectVertex + 3) / 4;
	assert(numBoneX4 <= 2 && "Maximum number of vertex associations greater than 8");
	VertexLayout layout;
	layout
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bitangent)
		.Append(VertexLayout::Texture2D);
	for (UINT i = 0; i < 2; i++) {
		layout
			.Append(VertexLayout::BoneIndex);
	}
	for (UINT i = 0; i < 2; i++) {
		layout
			.Append(VertexLayout::BoneWeight);
	}
	Dvtx::VertexBuffer vbuf(std::move(layout));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		boneWeights[i].resize(8, std::pair<int, float>{-1,0.0f});
		dx::XMINT4 idx0(boneWeights[i][0].first,boneWeights[i][1].first,boneWeights[i][2].first,boneWeights[i][3].first);
		dx::XMINT4 idx1(boneWeights[i][4].first,boneWeights[i][5].first,boneWeights[i][6].first,boneWeights[i][7].first);
		dx::XMFLOAT4 weight0(boneWeights[i][0].second, boneWeights[i][1].second, boneWeights[i][2].second, boneWeights[i][3].second);
		dx::XMFLOAT4 weight1(boneWeights[i][4].second, boneWeights[i][5].second, boneWeights[i][6].second, boneWeights[i][7].second);

		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]),
			idx0,idx1,
			weight0,weight1
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


	std::vector<std::shared_ptr<Bindable>> bindablePtrs;
	if (mesh.mMaterialIndex >= 0)
	{
		using namespace std::string_literals;
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) {
			std::string a = texFileName.C_Str();
			for (char& c : a) {
				if (c == '/') {
					c = '\\';
				}
			}
			bindablePtrs.push_back(Texture::Resolve(gfx, filePatch + a.c_str()));
		}
		bindablePtrs.push_back(Sampler::Resolve(gfx));
	}
	auto meshTag = filePatch + "%" + mesh.mName.C_Str() + std::to_string(vbuf.Size());

	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "SkeletonMeshVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));
	auto a = vbuf.GetLayout().GetD3DLayout();

	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

	bindablePtrs.push_back(PixelShader::Resolve(gfx, "SkeletonMeshPS.cso"));

	return std::make_unique<SKMesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<SKNode> SkeletonMesh::ParseNode(int& nextId, const aiNode& node) noexcept
{
	namespace dx = DirectX;
	auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<SKNode>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);

	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}

void SkeletonMesh::RenewBoneInfo(std::string boneName, DirectX::XMMATRIX boneTransform)
{
	if (bones.find(boneName) != bones.end()) {
		bones[boneName].BoneTransform = boneTransform;
	}
	ParseBone(pRoot.get(), dx::XMMatrixIdentity());
}

void SkeletonMesh::ParseBone(SKNode* p, dx::XMMATRIX transform)
{
	if (bones.find(p->GetName()) != bones.end()) {
		bones[p->GetName()].FinalTransformation = bones[p->GetName()].BoneTransform * transform;
		transform = bones[p->GetName()].FinalTransformation;
	}
	for (auto& child : p->GetChild()) {
		ParseBone(static_cast<SKNode*>(child.get()), transform);
	}
}

SKMesh::SKMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
	:
	Mesh(gfx, std::move(bindPtrs))
{
}

SKNode::SKNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform)
	:
	Node(id,name,std::move(meshPtrs),transform)
{
}

void SKNode::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const
{
	const auto built =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx,built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

std::string SKNode::GetName()
{
	return this->name;
}
