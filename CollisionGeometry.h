#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Transform.h"
#include "Camera.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class CollisionGeometry :public Drawable 
{
public:
	struct CollisionRes 
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		uint16_t hitTriangleIndices[3];
		float hitDistance;
	};
	CollisionGeometry(Graphics& gfx) :pCBufColor(gfx) {};
	CollisionGeometry(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	virtual std::vector<CollisionRes> TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix = XMMatrixIdentity(),float posOffset = 0.0f);

	
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMFLOAT3 GetPos() noexcept;
	void SetColor(DirectX::XMFLOAT3 Color) noexcept;
	DirectX::XMFLOAT3 GetColor()noexcept;
	void SetSelect(bool IsSelected)noexcept;
	FTransform GetTransform();
	void SetTransform(FTransform&transform);
	void Draw(Graphics& gfx)const noexcept override;
	virtual void Bind(Graphics& gfx)noexcept;
public:

protected:
	bool Selected=false;
	std::unique_ptr<Dvtx::VertexBuffer> vertexBuffer;
	std::vector<uint16_t> indices;
	DirectX::XMFLOAT3 color = { 1.0f,1.0f,0.0f };
	struct Color
	{
		DirectX::XMFLOAT3 color;
		float padding;
	};
	Bind::PixelConstantBuffer<Color> pCBufColor;
	FTransform transform;
};


class Line:public CollisionGeometry
{
public:
	Line(Graphics& gfx,Camera&cam, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
	void Bind(Graphics& gfx)noexcept override;
	virtual std::vector<CollisionRes> TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix = XMMatrixIdentity(),float posOffset =0.0f) override;
	void Draw(Graphics& gfx)const noexcept override;

private:

private:
	Camera* cam;
	Graphics& gfx;
};

class WidthLine :public Line
{
public:
	WidthLine(Graphics& gfx, Camera& cam, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f }, int lineWidth = 1);
	virtual std::vector<CollisionRes> TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector, DirectX::XMMATRIX transformMatrix = XMMatrixIdentity(), float posOffset = 0.0f) override;
	void Draw(Graphics& gfx)const noexcept override;
	void Bind(Graphics& gfx)noexcept override;
	struct CbufData
	{
		XMMATRIX ViewProj;
		alignas(16) float r;
	};
	Bind::GeometryConstantBuffer<CbufData> gcBuf;
	float width;
};

class TriangelGeo:public CollisionGeometry
{
public:
	TriangelGeo(Graphics& gfx, Dvtx::VertexBuffer& _vertexBuffer, std::vector<uint16_t> _indices, DirectX::XMFLOAT3 _pos = { 0.0f,0.0f,0.0f });
	void Bind(Graphics& gfx)noexcept override;
	void Draw(Graphics& gfx)const noexcept override;
};
