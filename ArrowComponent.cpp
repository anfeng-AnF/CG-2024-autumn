#include "ArrowComponent.h"

ArrowComponent::ArrowComponent(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos)
{
	using namespace Bind;
	AddBind(VertexBuffer::Resolve(gfx, "Arrow", _vertexBuffer));
	AddBind(IndexBuffer::Resolve(gfx, "Arrow", _indices));
	AddBind(Bind::PixelShader::Resolve(gfx, "ArrowPS.cso"));

	auto pvs = Bind::VertexShader::Resolve(gfx, "ArrowVS.cso");
	auto pvsb = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(Bind::InputLayout::Resolve(gfx, _vertexBuffer.GetLayout(), pvsb));

	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	struct PSColorConstant
	{
		alignas(16) float scale;
	} colorScale;
	AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorScale));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

std::shared_ptr<ArrowComponent> ArrowComponent::ConstructHelper(Graphics& gfx, XMFLOAT3 color, DirectX::XMFLOAT3 _pos)
{
	std::string fileName = "Models\\arrow\\arrow.fbx";
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded
	);
	pScene->mNumMeshes;
	auto mesh = pScene->mMeshes[0];
	Dvtx::VertexBuffer vBuf(
		Dvtx::VertexLayout{}
		.Append(Dvtx::VertexLayout::Position3D)
		.Append(Dvtx::VertexLayout::Position3D)
	);
	for (int i = 0; i < mesh->mNumVertices; i++) {
		vBuf.EmplaceBack(
			*reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[i]),
			*reinterpret_cast<XMFLOAT3*>(&color)
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh->mNumFaces * 3);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return std::make_shared<ArrowComponent>(gfx, vBuf, indices, _pos);
}

DirectX::XMMATRIX ArrowComponent::GetTransformXM() const noexcept
{
	return DirectX::XMMATRIX();
}
