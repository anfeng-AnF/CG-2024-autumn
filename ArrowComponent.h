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
class ArrowComponent
{
public:
	ArrowComponent(Graphics& gfx,std::string filePath);

private:
	std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

private:
	FTransform transform;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};