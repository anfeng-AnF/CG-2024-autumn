#include "ArrowComponent.h"

ArrowComponent::ArrowComponent(Graphics& gfx, std::string filePath)
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
        this->ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials);
    }

}

std::unique_ptr<Mesh> ArrowComponent::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;
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
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
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

	bindablePtrs.push_back(std::make_shared<TransformCbuf>(gfx, *this));

    return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}
