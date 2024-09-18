#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"

class DebugLine:public Drawable
{
public:
	DebugLine(Graphics& gfx, DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3 color);
	DebugLine(Graphics& gfx, DirectX::XMFLOAT3 rayOrigin, DirectX::XMFLOAT3 rayDirection ,float length, DirectX::XMFLOAT3 color);
	void destory() override{
		for (auto bind : binds) {
			bind.reset();
		}
		Bind::Codex::Remove(id);
	};
private:
	// Í¨¹ý Drawable ¼Ì³Ð
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	std::string id;
};
