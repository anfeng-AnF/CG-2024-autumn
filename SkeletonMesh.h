#include "Mesh.h"

class SkMesh :public Mesh 
{
public:
	SkMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);

};

class Animation 
{

};


class SkeletonMesh
{
public:
	SkeletonMesh(Graphics& gfx, const std::string fileName,float scale = 1.0f);
	void Draw(Graphics& gfx) const;

private:
	std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::string filePatch = "..\\";
	std::unordered_map<std::string,Animation> animations;
};