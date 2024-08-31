#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"

class DebugSphere :public Drawable
{
public:
	DebugSphere(Graphics& gfx, DirectX::XMFLOAT3 color,DirectX::XMFLOAT3 pos, float r = 1.0f);

private:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color = { 1.0f,1.0f,0.0f };
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};
