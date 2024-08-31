#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "CollisionGeometry.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Transform.h"

using namespace DirectX;
class ArrowComponent:public Drawable
{
public:
	ArrowComponent(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
	static std::shared_ptr<ArrowComponent> ConstructHelper(Graphics&gfx, XMFLOAT3 color, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });

private:
	FTransform transform;

	// Í¨¹ý Drawable ¼Ì³Ð
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};