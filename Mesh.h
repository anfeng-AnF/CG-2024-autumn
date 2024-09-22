#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
protected:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform);
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	int GetId() const noexcept;
	void AddChild(std::unique_ptr<Node> pChild);
	std::vector<std::unique_ptr<Node>>& GetChild() noexcept;
	std::vector<Mesh*>& GetMeshPtrs()noexcept;
	DirectX::XMMATRIX GetTransform()noexcept;
protected:
	void ShowTree(std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept;
protected:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string fileName);
	void Draw(Graphics& gfx) const;
	void ShowWindow(const char* windowName = nullptr) noexcept;
	~Model() noexcept;
private:
	std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
	std::string filePatch="..\\";
};