#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "CollisionGeometry.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Camera.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Transform.h"
#include "imguiManager.h"

using namespace DirectX;

class cMesh:public Mesh
{
	friend class TransformCtrlComponent;
public:
	cMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs,
		Dvtx::VertexBuffer vbuf, std::vector<uint16_t> indices, std::string meshName);
	void SetName(std::string& s);
	std::string GetName();
	void Bind(Graphics& gfx)const;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const override;
	void SetSelect(bool IsSelect);
private:
	bool selected=false;
	std::string meshName;
	std::unique_ptr<Dvtx::VertexBuffer> vertexBuffer;
	std::vector<uint16_t> indices;
	XMFLOAT4 colorScale = {0.7f,0.7f,0.7f,0.7f};
	mutable Bind::PixelConstantBuffer<XMFLOAT4> pcBufColorScale;
};

class TransformCtrlComponent:public CollisionGeometry
{
public:
	TransformCtrlComponent(Graphics& gfx,Camera&cam, std::string filePath, float _loadScale = 0.01);
	void Draw(Graphics& gfx) const noexcept override;
	std::pair<CollisionRes, cMesh*> TraceByLineGetNearestMesh(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector);
	void Bind(Graphics& gfx)noexcept override;
	FTransform GetTransform();
	void SetTransform(FTransform _transform);
	float GetDynamicScaling(Graphics& gfx, Camera& cam)const;
private:
	std::unique_ptr<cMesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
	void dfs(std::unique_ptr<Node>& node, DirectX::XMMATRIX transform);
private:
	const float loadScale;
	FTransform transform;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<cMesh>> meshPtrs;

	//trace by line vals
	DirectX::XMFLOAT3 tlineBeginPos;
	DirectX::XMFLOAT3 tlineVector;
	std::vector<std::pair<cMesh*, std::vector<CollisionGeometry::CollisionRes>>>hitResults;

	std::string filePath;
	Camera* cam;
	Graphics* gfx;
};