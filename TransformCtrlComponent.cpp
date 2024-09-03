#include "TransformCtrlComponent.h"
#include"iomanip"
cMesh::cMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs,
	Dvtx::VertexBuffer vbuf, std::vector<uint16_t> indices, std::string meshName)
	:
	Mesh(gfx, std::move(bindPtrs)),
	vertexBuffer(std::make_unique<Dvtx::VertexBuffer>(vbuf)),
	indices(std::move(indices)),
	meshName(meshName)
{
}
void cMesh::SetName(std::string& s)
{
	this->meshName = s;
}

std::string cMesh::GetName()
{
	return this->meshName;
}

TransformCtrlComponent::TransformCtrlComponent(Graphics& gfx, std::string filePath, float _loadScale) :
	loadScale(_loadScale),
	CollisionGeomerty(gfx)
{
    Assimp::Importer imp;
    const auto pScene = imp.ReadFile(filePath.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded
    );
    assert(pScene && "pScene is nullptr. Can't open file.");

    auto Material = pScene->mMaterials;

    for (int i = 0; i < pScene->mNumMeshes; i++) {
		meshPtrs.push_back(this->ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
    }
	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}


void TransformCtrlComponent::Draw(Graphics& gfx) const noexcept
{
	auto depth = Bind::DepthStencilState(gfx);
	//depth.Bind(gfx);
	pRoot->Draw(gfx, transform.GetMatrix());
	//depth.UnBind(gfx);
}

std::pair<CollisionGeomerty::CollisionRes, cMesh*> TransformCtrlComponent::TraceByLineGetNearestMesh(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
{
	CollisionGeomerty::CollisionRes hitResRet;
	hitResults.clear();
	tlineBeginPos = lineBeginPos;
	tlineVector = lineVector;
	for (auto& rootChild : pRoot->GetChild()) {
		dfs(rootChild, transform.GetMatrix());
	}

	int minDistance = INT32_MAX;
	cMesh* nearestMesh = nullptr;
	for (auto& hitRes : hitResults) {
		for (auto& hitInfo : hitRes.second) {
			if (hitInfo.hitDistance < minDistance) {
				minDistance = hitInfo.hitDistance;
				nearestMesh = hitRes.first;
				hitResRet = hitInfo;
			}
		}
	}
	hitResults.clear();

	return { hitResRet,nearestMesh };
}

std::unique_ptr<cMesh> TransformCtrlComponent::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;


	OutputDebugStringA(mesh.mName.C_Str());
	OutputDebugStringA("\n");

	aiColor3D acolor;
	if (pMaterials[mesh.mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, acolor) != AI_SUCCESS) {
		acolor = { 1.0f,1.0f,1.0f };
	}
	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Float3Color)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		float float3[] = { mesh.mVertices[i].x * loadScale,mesh.mVertices[i].y * loadScale,mesh.mVertices[i].z * loadScale };
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(float3),
			*reinterpret_cast<dx::XMFLOAT3*>(&acolor)
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
	const char* name = mesh.mName.C_Str();
	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, name, vbuf));
	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, name, indices));
	bindablePtrs.push_back(Bind::PixelShader::Resolve(gfx, "ArrowPS.cso"));

	auto pvs = Bind::VertexShader::Resolve(gfx, "ArrowVS.cso");
	auto pvsb = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));
	auto a = vbuf.GetLayout().GetD3DLayout();
	bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsb));

	bindablePtrs.push_back(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color;
		float padding;
	} colorConst;
	bindablePtrs.push_back(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst));

	return std::make_unique<cMesh>(gfx, std::move(bindablePtrs), std::move(vbuf), indices, mesh.mName.C_Str());
}

std::unique_ptr<Node> TransformCtrlComponent::ParseNode(int& nextId, const aiNode& node) noexcept
{

	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}

void TransformCtrlComponent::dfs(std::unique_ptr<Node>& node, DirectX::XMMATRIX transform)
{
	XMMATRIX applyTransform = node->GetTransform()*transform;
	if (1) {
		//
		std::ostringstream oss;
		// 解析平移
		XMFLOAT3 translation;
		translation.x = applyTransform.r[3].m128_f32[0];
		translation.y = applyTransform.r[3].m128_f32[1];
		translation.z = applyTransform.r[3].m128_f32[2];

		// 解析缩放
		XMFLOAT3 scale;
		scale.x = sqrtf(applyTransform.r[0].m128_f32[0] * applyTransform.r[0].m128_f32[0] +
			applyTransform.r[1].m128_f32[0] * applyTransform.r[1].m128_f32[0] +
			applyTransform.r[2].m128_f32[0] * applyTransform.r[2].m128_f32[0]);

		scale.y = sqrtf(applyTransform.r[0].m128_f32[1] * applyTransform.r[0].m128_f32[1] +
			applyTransform.r[1].m128_f32[1] * applyTransform.r[1].m128_f32[1] +
			applyTransform.r[2].m128_f32[1] * applyTransform.r[2].m128_f32[1]);

		scale.z = sqrtf(applyTransform.r[0].m128_f32[2] * applyTransform.r[0].m128_f32[2] +
			applyTransform.r[1].m128_f32[2] * applyTransform.r[1].m128_f32[2] +
			applyTransform.r[2].m128_f32[2] * applyTransform.r[2].m128_f32[2]);

		// 解析旋转
		XMMATRIX rotationMatrix = applyTransform;
		rotationMatrix.r[0] = XMVectorScale(rotationMatrix.r[0], 1.0f / scale.x);
		rotationMatrix.r[1] = XMVectorScale(rotationMatrix.r[1], 1.0f / scale.y);
		rotationMatrix.r[2] = XMVectorScale(rotationMatrix.r[2], 1.0f / scale.z);

		// 将旋转矩阵转换为欧拉角
		XMFLOAT3 eulerAngles;
		FTransform t;
		t.rotation = XMQuaternionRotationMatrix(rotationMatrix);
		eulerAngles = t.GetRotationEuler();
		// 输出结果
		auto& meshPtrs = node->GetMeshPtrs();
		if(meshPtrs.size())
		oss << dynamic_cast<cMesh*>(meshPtrs[0])->GetName() << std::endl;
		oss << std::fixed << std::setprecision(6);
		oss << "Translation: (" << translation.x << ", " << translation.y << ", " << translation.z << ")\n";
		oss << "Rotation (Euler angles in radians): (" << eulerAngles.x << ", " << eulerAngles.y << ", " << eulerAngles.z << ")\n";
		oss << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")\n";

		// 打印或输出 oss.str()
		std::string result = oss.str();
		OutputDebugStringA(result.c_str());
		//
	}
	for (auto& child : node->GetChild()) {
		dfs(child, applyTransform);
	}
	auto& meshPtrs= node->GetMeshPtrs();
	for (auto& pMesh : meshPtrs) {
		auto pCMesh = dynamic_cast<cMesh*>(pMesh);
		if (pCMesh) {
			auto meshTransform = applyTransform * pCMesh->GetTransformXM();
			this->vertexBuffer = std::move(pCMesh->vertexBuffer);
			this->indices = std::move(pCMesh->indices);
			auto hitRes = CollisionGeomerty::TraceByLine(tlineBeginPos, tlineVector, meshTransform);
			if (hitRes.size()) {
				hitResults.push_back({ pCMesh,hitRes });
			}
			pCMesh->indices = std::move(this->indices);
			pCMesh->vertexBuffer = std::move(this->vertexBuffer);
		}
	}
}
