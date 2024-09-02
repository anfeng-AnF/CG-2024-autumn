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

using namespace DirectX;

class cMesh:public Mesh
{
	friend class ArrowComponent;
public:
	cMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs,
		Dvtx::VertexBuffer vbuf, std::vector<uint16_t> indices);

private:
	std::unique_ptr<Dvtx::VertexBuffer> vertexBuffer;
	std::vector<uint16_t> indices;
};

class ArrowComponent:public CollisionGeomerty
{
public:
	ArrowComponent(Graphics& gfx,std::string filePath,float _loadScale=0.01);
	void Draw(Graphics& gfx) const noexcept override;
	std::vector<CollisionRes> TraceByLine(DirectX::XMFLOAT3 lineBeginPos, DirectX::XMFLOAT3 lineVector)override;

private:
	std::unique_ptr<cMesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

private:
	const float loadScale;
	FTransform transform;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<cMesh>> meshPtrs;
};