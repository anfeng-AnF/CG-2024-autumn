#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class CollisionGeomerty :public Drawable 
{
public:
	struct CollisionRes 
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		uint16_t hitTriangleIndices[3];
		float hitDistance;
	};
	CollisionGeomerty(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos={0.0f,0.0f,0.0f});
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	std::vector<CollisionRes> TraceByLine(DirectX::XMFLOAT3 lineBeginPos,DirectX::XMFLOAT3 lineVector);
	
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMFLOAT3 GetPos() noexcept;
	void SetColor(DirectX::XMFLOAT3 Color) noexcept;
	void Bind(Graphics& gfx)noexcept;
	void SetSelect(bool IsSelected)noexcept;
public:

private:
	bool Selected=false;
	DirectX::XMFLOAT3 pos;
	Dvtx::VertexBuffer vertexBuffer;
	std::vector<uint16_t> indices;
	DirectX::XMFLOAT3 color = { 1.0f,1.0f,0.0f };
	struct Color
	{
		DirectX::XMFLOAT3 color;
		float padding;
	};
	Bind::PixelConstantBuffer<Color> pCBufColor;
};