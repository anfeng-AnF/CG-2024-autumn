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
	struct CollisionRes 
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};
public:
	CollisionGeomerty(Graphics &gfx,Dvtx::VertexBuffer& Vertexbuffer, std::vector<uint16_t> indices);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	std::vector<CollisionRes> TraceByLine(DirectX::XMFLOAT3 lineBeginPos,DirectX::XMFLOAT3 lineVector);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;

public:


private:
	bool Selected=false;
	DirectX::XMFLOAT3 pos;
	Dvtx::VertexBuffer vertexBuffer;
	std::vector<uint16_t> indices;

};