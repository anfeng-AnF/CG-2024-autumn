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
	std::vector<std::shared_ptr<Bind::Bindable>>& GetBinds();
protected:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class StaticMesh;
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
	virtual void ShowTree(std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept;
protected:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class StaticMesh
{
public:
	StaticMesh(Graphics& gfx, const std::string fileName);
	void Draw(Graphics& gfx) const;
	void ShowWindow(const char* windowName = nullptr) noexcept;
	~StaticMesh() noexcept;
	template<typename ShaderType>
	void SetShader(std::string newFileName, std::string perviousFileName);
private:
	std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
	std::string filePatch="..\\";
	Graphics& gfx;
};
template<typename ShaderType>
inline void StaticMesh::SetShader(std::string newFileName, std::string perviousFileName)
{
	using namespace std::string_literals;
	for (auto& mesh : meshPtrs) {
		for (auto& bind : mesh->GetBinds()) {
			auto perBindName = bind->GetUID();
			auto typeName = typeid(ShaderType).name();
			if (perBindName.find(typeid(ShaderType).name() + "#"s + perviousFileName)!= std::string::npos) {
				bind = ShaderType::Resolve(gfx, newFileName);
			}
		}
	}
}
