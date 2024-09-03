#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "CollisionGeometry.h"
#include "Vertex.h"
#include "Mesh.h"
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
private:
	std::string meshName;
	std::unique_ptr<Dvtx::VertexBuffer> vertexBuffer;
	std::vector<uint16_t> indices;
};

class TransformCtrlComponent:public CollisionGeomerty
{
public:
	TransformCtrlComponent(Graphics& gfx,std::string filePath,float _loadScale=0.01);
	void Draw(Graphics& gfx) const noexcept override;
	std::pair<CollisionRes, cMesh*> TraceByLineGetNearestMesh(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector);

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
	std::vector<std::pair<cMesh*, std::vector<CollisionGeomerty::CollisionRes>>>hitResults;
};