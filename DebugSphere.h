#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"

class DebugSphere :public Drawable
{
public:
	DebugSphere(Graphics& gfx, DirectX::XMFLOAT3 color,DirectX::XMFLOAT3 pos, float r = 0.3f);
	void destory() override {
		for (auto bind : binds) {
			bind.reset();
		}
		Bind::Codex::Remove(id);
	};
	void SetPos(DirectX::XMFLOAT3 position) {
		pos = position;
	};
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color = { 1.0f,1.0f,0.0f };
	std::string id;
};
