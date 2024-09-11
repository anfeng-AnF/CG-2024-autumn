#include "TransformCtrlComponent.h"
#include"iomanip"
cMesh::cMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs,
	Dvtx::VertexBuffer vbuf, std::vector<uint16_t> indices, std::string meshName)
	:
	Mesh(gfx, std::move(bindPtrs)),
	vertexBuffer(std::make_unique<Dvtx::VertexBuffer>(vbuf)),
	indices(std::move(indices)),
	meshName(meshName),
	pcBufColorScale(gfx, 3u)
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

void cMesh::Bind(Graphics& gfx)const
{
	DirectX::XMFLOAT4 dataCopy;
	if (selected)dataCopy = colorScale;
	else dataCopy = { 1.0f,1.0f,1.0f,1.0f };
	pcBufColorScale.Update(gfx, dataCopy);
	pcBufColorScale.Bind(gfx);
}

void cMesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const
{
	Bind(gfx);
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

void cMesh::SetSelect(bool IsSelect)
{
	selected = IsSelect;
}

TransformCtrlComponent::TransformCtrlComponent(Graphics& gfx,Camera&cam, std::string filePath, float _loadScale) :
	loadScale(_loadScale),
	CollisionGeometry(gfx),
	filePath(filePath),
	cam(&cam),
	gfx(&gfx)
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
	float scale = GetDynamicScaling(gfx,*cam);

	FTransform dynamicTransform = FTransform(
		transform.position,
		{ scale, scale, scale },
		transform.rotation
	);
	
	pRoot->Draw(gfx, dynamicTransform.GetMatrix());
}


std::pair<CollisionGeometry::CollisionRes, cMesh*> TransformCtrlComponent::TraceByLineGetNearestMesh(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)
{
	CollisionGeometry::CollisionRes hitResRet;
	hitResults.clear();
	tlineBeginPos = lineBeginPos;
	tlineVector = lineVector;

	float scale = GetDynamicScaling(*gfx,*cam);

	FTransform dynamicTransform = FTransform(
		transform.position,
		{ scale, scale, scale },
		transform.rotation
	);

	for (auto& rootChild : pRoot->GetChild()) {
		dfs(rootChild, dynamicTransform.GetMatrix());
	}

	float minDistance = D3D11_FLOAT32_MAX;
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

void TransformCtrlComponent::Bind(Graphics& gfx) noexcept
{

}

FTransform TransformCtrlComponent::GetTransform()
{
	return transform;
}

void TransformCtrlComponent::SetTransform(FTransform _transform)
{
	transform = _transform;
}

// 计算动态缩放因子
float TransformCtrlComponent::GetDynamicScaling(Graphics& gfx, Camera& cam) const
{

	XMVECTOR objectPosition = transform.position;
	XMMATRIX viewMatrix = cam.GetMatrix();
	XMMATRIX projectionMatrix = gfx.GetProjection();
	if (!IsPerspectiveMatrix(projectionMatrix)) {
		return 1.0f;
	}
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	XMVECTOR pos4 = XMVectorSetW(objectPosition, 1.0f);
	XMVECTOR clipPos = XMVector4Transform(pos4, viewProjectionMatrix);

	float depth = clipPos.m128_f32[3];

	// 计算缩放因子
	float referenceDepth = 20.0f;

	return depth/ referenceDepth ;
}
std::unique_ptr<cMesh> TransformCtrlComponent::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

#ifdef DEBUG
	OutputDebugStringA(mesh.mName.C_Str());
	OutputDebugStringA("\n");
#endif // DEBUG

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
	std::string name = mesh.mName.C_Str();
	name += filePath;
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

	for (auto& child : node->GetChild()) {
		dfs(child, applyTransform);
	}
	auto& meshPtrs= node->GetMeshPtrs();
	for (auto& pMesh : meshPtrs) {
		auto pCMesh = dynamic_cast<cMesh*>(pMesh);
		if (pCMesh) {
			this->vertexBuffer = std::move(pCMesh->vertexBuffer);
			this->indices = std::move(pCMesh->indices);
			auto hitRes = CollisionGeometry::TraceByLine(tlineBeginPos, tlineVector, applyTransform);
			if (hitRes.size()) {
				hitResults.push_back({ pCMesh,hitRes });
			}
			pCMesh->indices = std::move(this->indices);
			pCMesh->vertexBuffer = std::move(this->vertexBuffer);
		}
	}
}
