#include"Mesh.h"

struct BoneInfo {
	DirectX::XMMATRIX BoneOffset;
	DirectX::XMMATRIX BoneTransform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX FinalTransformation;
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
private:
};

class Animate
{
public:

};

class SkeletonMesh
{
public:
	SkeletonMesh(Graphics& gfx, const std::string fileName);
	void Draw(Graphics& gfx);
	void Bind(Graphics& gfx);
private:
	std::unique_ptr<SKMesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<SKNode> ParseNode(int& nextId, const aiNode& node) noexcept;
	void RenewBoneInfo(std::string boneName, DirectX::XMMATRIX boneTransform);
	void ParseBone(SKNode*p,DirectX::XMMATRIX transform);
private:
	std::unique_ptr<SKNode> pRoot;
	std::vector<std::unique_ptr<SKMesh>> meshPtrs;

	std::string filePatch = "..\\";

	std::unordered_map<std::string, BoneInfo> bones;
	std::vector<std::string> bonesName;

	struct VCBufData
	{
		DirectX::XMMATRIX bones[1024];
	};
	std::unique_ptr<VCBufData> data = std::make_unique<VCBufData>();
	Bind::VertexConstantBuffer<VCBufData> VCbufBones;
};