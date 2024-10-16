#pragma once
#include"Mesh.h"

struct BoneInfo {
	DirectX::XMMATRIX BoneOffset = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX BoneTransform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX defaultPose = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX FinalTransformation = DirectX::XMMatrixIdentity();
};

class SKMesh :public Mesh
{
public:
	SKMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
private:
};

class SKNode :public Node
{
public:
	SKNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const override;
	std::string GetName();
	void SetTransform(DirectX::XMMATRIX transform);
private:
};

class SkeletonMesh
{
public:
	SkeletonMesh(Graphics& gfx, const std::string fileName);
	void Draw(Graphics& gfx,DirectX::FXMMATRIX Transform=DirectX::XMMatrixIdentity());
	void Bind(Graphics& gfx);
	void CtrlWnd(Graphics& gfx);
	void SetBonesTransform(std::unordered_map<std::string, DirectX::XMMATRIX>& transforms);
	template<typename ShaderType>
	void SetShader(std::string newFileName, std::string perviousFileName)
	{
		using namespace std::string_literals;
		for (auto& mesh : meshPtrs) {
			for (auto& bind : mesh->GetBinds()) {
				auto perBindName = bind->GetUID();
				auto typeName = typeid(ShaderType).name();
				if (perBindName.find(typeid(ShaderType).name() + "#"s + perviousFileName) != std::string::npos) {
					bind = ShaderType::Resolve(gfx, newFileName);
				}
			}
		}
	}
private:
	std::unique_ptr<SKMesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<SKNode> ParseNode(int& nextId, const aiNode& node) noexcept;
	void RenewBoneInfo(std::string boneName, DirectX::XMMATRIX boneTransform);
	void UpdateBoneInfo(SKNode*p,DirectX::XMMATRIX transform);
private:
	UINT numNode = 0;
	std::unique_ptr<SKNode> pRoot;
	std::vector<std::unique_ptr<SKMesh>> meshPtrs;

	std::string filePatch = "..\\";

public:
	std::unordered_map<std::string, BoneInfo> bones;
	std::unordered_map<std::string, DirectX::XMMATRIX> ctrlInfo;
	std::vector<std::string> bonesName;

	struct VCBufData
	{
		DirectX::XMMATRIX bones[1024];
	};
	std::unique_ptr<VCBufData> data = std::make_unique<VCBufData>();
	Bind::VertexConstantBuffer<VCBufData> VCbufBones;
	Graphics& gfx;
};
