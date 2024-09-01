#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"

class DebugLine:public Drawable
{
public:
	DebugLine(Graphics&gfx,DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end,DirectX::XMFLOAT3 color);

private:

	// Í¨¹ý Drawable ¼Ì³Ð
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

};
