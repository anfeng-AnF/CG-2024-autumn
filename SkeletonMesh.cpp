#include "SkeletonMesh.h"
#include "Transform.h"
#include "DebugGraphsMannger.h"
#include "imguiManager.h"
#include <stack>
#pragma warning(disable : 4996)

namespace dx = DirectX;
DirectX::XMMATRIX AiMatrixToXMMATRIX(const aiMatrix4x4& aiMat) {
	DirectX::XMMATRIX xmMat = DirectX::XMMATRIX(
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	);
	return xmMat;
}

SkeletonMesh::SkeletonMesh(Graphics& gfx, const std::string fileName)
	:
	VCbufBones(gfx,1u),
	gfx(gfx)
{
	//get and save file patch
	std::size_t pos = fileName.find_last_of('\\');
	if (pos != std::string::npos) {
		filePatch = fileName.substr(0, pos + 1);
	}


	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}
	
	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
	UpdateBoneInfo(pRoot.get(), dx::XMMatrixIdentity());
}

void SkeletonMesh::Draw(Graphics& gfx)
{
	this->Bind(gfx);
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	for (auto& line : boneLine) {
		DebugGraphsMannger::GetInstence().AddGeo(std::make_shared<DebugLine>(gfx, line.first, line.second, XMFLOAT3(0, 1, 1)), 0.0f);
	}
}

void SkeletonMesh::Bind(Graphics& gfx)
{
	assert(bonesName.size() <= 1024 && "Bone count over 1024");
	for (int i = 0; i < bonesName.size(); i++) {
		data->bones[i] =dx::XMMatrixTranspose(bones[bonesName[i]].FinalTransformation);
	}
	VCbufBones.Update(gfx, *data.get());
	VCbufBones.Bind(gfx);
}

void SkeletonMesh::CtrlWnd(Graphics& gfx)
{
	struct StackItem {
		Node* node;
		bool expanded;
		size_t childIndex;
	};
	static std::optional<int> selectedIndex=0;
	static Node* pSelectedNode = pRoot.get();
	static std::unordered_map<int, FTransform> transforms;

	// ջ������ģ��ݹ顣��ʼ״̬ջ�д洢���ڵ�
	std::stack<StackItem> stack;
	stack.push({ pRoot.get(), false, 0});
	ImGui::Begin(pRoot->GetName().c_str());
	ImGui::Columns(2, nullptr, true);

	while (!stack.empty()) {
		auto& current = stack.top();  // ��ȡջ��Ԫ��

		// ����ýڵ㻹û��չ��������ִ�нڵ��չʾ�߼�
		if (!current.expanded) {
			// ���ɵ�ǰ�ڵ�ı�־λ
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((current.node->GetId() == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
				| ((current.node->GetChild().size()== 0) ? ImGuiTreeNodeFlags_Leaf : 0);

			// ��Ⱦ�ڵ㣬���ж��Ƿ�չ��
			current.expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)current.node->GetId(), node_flags, static_cast<SKNode*>(current.node)->GetName().c_str()
			);

			// �������˸ýڵ㣬����ѡ��״̬
			if (ImGui::IsItemClicked()) {
				selectedIndex = current.node->GetId();
				pSelectedNode = const_cast<Node*>(current.node);
			}
		}

		// ����ڵ�չ�������������ӽڵ�
		if (current.expanded && current.childIndex < current.node->GetChild().size()) {
			// �ݹ鴦���ӽڵ㣺���ӽڵ�ѹ��ջ��
			stack.push({ current.node->GetChild()[current.childIndex].get(), false, 0});
			current.childIndex++;  // �����ӽڵ�����������´�Ҫ������ӽڵ�
		}
		else {
			// ��ǰ�ڵ���ӽڵ㴦����ϣ�����ջ��Ԫ��
			if (current.expanded) {
				ImGui::TreePop();
			}
			stack.pop();
		}
	}
	auto pSkNode = static_cast<SKNode*>(pSelectedNode);
	ImGui::NextColumn();
	if (transforms.find(selectedIndex.value()) == transforms.end()) {
		transforms[selectedIndex.value()] = dx::XMMatrixIdentity();
	}
	FTransform& t=transforms[selectedIndex.value()];
	bool Changed = false;
	ImGui::Text("Position");
	Changed |= ImGui::SliderFloat("X", &reinterpret_cast<float*>(&t.position)[0], -20.0f, 20.0f);
	Changed |= ImGui::SliderFloat("Y", &reinterpret_cast<float*>(&t.position)[1], -20.0f, 20.0f);
	Changed |= ImGui::SliderFloat("Z", &reinterpret_cast<float*>(&t.position)[2], -20.0f, 20.0f);
	ImGui::Text("Orientation");
	Changed |= ImGui::SliderFloat("w", &reinterpret_cast<float*>(&t.rotation)[3], -2.0f, 2.0f);
	Changed |= ImGui::SliderFloat("x", &reinterpret_cast<float*>(&t.rotation)[0], -2.0f, 2.0f);
	Changed |= ImGui::SliderFloat("y", &reinterpret_cast<float*>(&t.rotation)[1], -2.0f, 2.0f);
	Changed |= ImGui::SliderFloat("z", &reinterpret_cast<float*>(&t.rotation)[2], -2.0f, 2.0f);
	t.rotation = dx::XMVector4Normalize(t.rotation);
	ctrlInfo[pSkNode->GetName()] =t.GetRotationMatrix() * t.GetTranslateMatrix();
	if (bones.find(pSkNode->GetName()) != bones.end()) {
		DirectX::XMFLOAT4X4 floatMatrix;
		DirectX::XMStoreFloat4x4(&floatMatrix, bones[pSkNode->GetName()].BoneTransform);
		//ImGui::Text("Matrix:");
		//ImGui::Text("[ %.3f, %.3f, %.3f, %.3f ]", floatMatrix._11, floatMatrix._12, floatMatrix._13, floatMatrix._14);
		//ImGui::Text("[ %.3f, %.3f, %.3f, %.3f ]", floatMatrix._21, floatMatrix._22, floatMatrix._23, floatMatrix._24);
		//ImGui::Text("[ %.3f, %.3f, %.3f, %.3f ]", floatMatrix._31, floatMatrix._32, floatMatrix._33, floatMatrix._34);
		//ImGui::Text("[ %.3f, %.3f, %.3f, %.3f ]", floatMatrix._41, floatMatrix._42, floatMatrix._43, floatMatrix._44);
		auto trs = FTransform(ctrlInfo[pSkNode->GetName()]*bones[pSkNode->GetName()].BoneTransform);
		XMFLOAT3 pos, scl;
		XMStoreFloat3(&pos, trs.position);
		XMStoreFloat3(&scl, trs.scale);

		// ��ȡŷ������ת
		auto rot = trs.rotation; // ���� GetRotationEuler() ���� XMFLOAT3

		// ��ʾ position, rotation, scale
		ImGui::Text("FTransform:");

		// ��ʾλ�� (Position)
		ImGui::Text("Position: [%.3f, %.3f, %.3f]", pos.x, pos.y, pos.z);

		// ��ʾ��ת (Rotation in Euler angles)
		ImGui::Text("Rotation: [%.3f, %.3f, %.3f, %.3f]", rot.m128_f32[0], rot.m128_f32[1], rot.m128_f32[2], rot.m128_f32[3]);

		// ��ʾ���� (Scale)
		ImGui::Text("Scale: [%.3f, %.3f, %.3f]", scl.x, scl.y, scl.z);
	}

	ImGui::End();

	if (Changed) {
		boneLine.clear();
		UpdateBoneInfo(pRoot.get(), dx::XMMatrixIdentity());
	}
}

void SkeletonMesh::SetBonesTransform(std::unordered_map<std::string, DirectX::XMMATRIX>& transforms)
{
	static int a = 0;
	a = 0;
	for (auto bone : transforms) {
		if (this->bones.find(bone.first) != bones.end()) {
			bones[bone.first].BoneTransform = bone.second;
		}
		else
		{
			a++;
			bones[bone.first].BoneTransform = bone.second;
		}
	}
	boneLine.clear();
	this->UpdateBoneInfo(pRoot.get(), dx::XMMatrixIdentity());
}

std::unique_ptr<SKMesh> SkeletonMesh::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

	//vertexId<bones<pair<boneid,weight>>>
	std::vector<std::vector<std::pair<int, float>>> boneWeights(mesh.mNumVertices);

	assert(!(bonesName.size() != 0 && bonesName.size() != mesh.mNumBones)&&"Using multiple skeletons for one model is not supported at this time");
	bonesName.resize(mesh.mNumBones);
	auto bones = mesh.mBones;
	for (UINT i = 0; i < mesh.mNumBones; i++) {

		//process bone name
		bonesName[i] = (bones[i]->mName.C_Str());
		assert(!(bonesName.size() != 0 && bonesName[i] != bones[i]->mName.C_Str()) && "Using multiple skeletons for one model is not supported at this time");

		BoneInfo boneInfo;
		boneInfo.BoneOffset = AiMatrixToXMMATRIX(bones[i]->mOffsetMatrix);
		this->bones[std::string(bones[i]->mName.C_Str())] = boneInfo;

		auto weights = bones[i]->mWeights;
		for (UINT j = 0; j < bones[i]->mNumWeights; j++) {
			boneWeights[weights[j].mVertexId].push_back({ i, weights[j].mWeight });
		}
	}
	UINT maxBoneAffectVertex = 0;
	for (const auto& v : boneWeights) {
		maxBoneAffectVertex = max(maxBoneAffectVertex, v.size());
	}
	int numBoneX4 = (maxBoneAffectVertex + 3) / 4;
	assert(numBoneX4 <= 2 && "Maximum number of vertex associations greater than 8");
	VertexLayout layout;
	layout
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bitangent)
		.Append(VertexLayout::Texture2D);
	for (UINT i = 0; i < 2; i++) {
		layout
			.Append(VertexLayout::BoneIndex);
	}
	for (UINT i = 0; i < 2; i++) {
		layout
			.Append(VertexLayout::BoneWeight);
	}
	Dvtx::VertexBuffer vbuf(std::move(layout));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		boneWeights[i].resize(8, std::pair<int, float>{-1,0.0f});
		dx::XMINT4 idx0(boneWeights[i][0].first,boneWeights[i][1].first,boneWeights[i][2].first,boneWeights[i][3].first);
		dx::XMINT4 idx1(boneWeights[i][4].first,boneWeights[i][5].first,boneWeights[i][6].first,boneWeights[i][7].first);
		dx::XMFLOAT4 weight0(boneWeights[i][0].second, boneWeights[i][1].second, boneWeights[i][2].second, boneWeights[i][3].second);
		dx::XMFLOAT4 weight1(boneWeights[i][4].second, boneWeights[i][5].second, boneWeights[i][6].second, boneWeights[i][7].second);

		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]),
			idx0,idx1,
			weight0,weight1
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}


	std::vector<std::shared_ptr<Bindable>> bindablePtrs;
	if (mesh.mMaterialIndex >= 0)
	{
		using namespace std::string_literals;
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) {
			std::string a = texFileName.C_Str();
			for (char& c : a) {
				if (c == '/') {
					c = '\\';
				}
			}
			bindablePtrs.push_back(Texture::Resolve(gfx, filePatch + a.c_str()));
		}
		bindablePtrs.push_back(Sampler::Resolve(gfx));
	}
	auto meshTag = filePatch + "%" + mesh.mName.C_Str() + std::to_string(vbuf.Size());

	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "SkeletonMeshVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));
	auto a = vbuf.GetLayout().GetD3DLayout();

	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

	bindablePtrs.push_back(PixelShader::Resolve(gfx, "SkeletonMeshPS.cso"));

	return std::make_unique<SKMesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<SKNode> SkeletonMesh::ParseNode(int& nextId, const aiNode& node) noexcept
{
	namespace dx = DirectX;
	auto transform =dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<SKNode>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	numNode++;
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	std::string curName = node.mName.C_Str();
	if (bones.find(curName) != bones.end()) {
		if (node.mParent && bones.find(std::string(node.mParent->mName.C_Str())) != bones.end()) {
			std::string parentName = node.mParent->mName.C_Str();
			bones[curName].defaultPose = 
				dx::XMMatrixInverse(nullptr, bones[curName].BoneOffset)
				* bones[parentName].BoneOffset;
			bones[curName].BoneTransform = bones[curName].defaultPose;
		}
	}
	ctrlInfo[curName] = dx::XMMatrixIdentity();
	return pNode;
}

void SkeletonMesh::RenewBoneInfo(std::string boneName, DirectX::XMMATRIX boneTransform)
{
	if (bones.find(boneName) != bones.end()) {
		bones[boneName].BoneTransform = boneTransform;
	}
	UpdateBoneInfo(pRoot.get(), dx::XMMatrixIdentity());
}

void SkeletonMesh::UpdateBoneInfo(SKNode* p, dx::XMMATRIX transform)
{
	XMMATRIX next = p->GetTransform();
	if (bones.find(p->GetName()) != bones.end()) {
		next = p->GetTransform()*ctrlInfo[p->GetName()]*bones[p->GetName()].BoneTransform;
	}
	next = next * transform;
	auto t = FTransform(next).position;
	auto pt = FTransform(transform).position;
	std::string name = p->GetName();
	boneLine.push_back({
			XMFLOAT3(dx::XMVectorGetX(t), dx::XMVectorGetY(t),dx::XMVectorGetZ(t)),
			XMFLOAT3(dx::XMVectorGetX(pt), dx::XMVectorGetY(pt), dx::XMVectorGetZ(pt))
		});

		if (bones.find(p->GetName()) != bones.end()) {
		bones[p->GetName()].FinalTransformation = bones[p->GetName()].BoneOffset * next;
	}
	for (auto& child : p->GetChild()) {
		UpdateBoneInfo(static_cast<SKNode*>(child.get()), next);
	}
}

SKMesh::SKMesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
	:
	Mesh(gfx, std::move(bindPtrs))
{
}

SKNode::SKNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform)
	:
	Node(id,name,std::move(meshPtrs),transform)
{
}

void SKNode::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const
{
	const auto built =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		accumulatedTransform;
	for (const auto pm : meshPtrs)
	{
		auto a = FTransform(built);
		pm->Draw(gfx, dx::XMMatrixIdentity());
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

std::string SKNode::GetName()
{
	return this->name;
}

