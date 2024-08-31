#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"

class Line:public Drawable
{
public:
	Line(Graphics&gfx,DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end,DirectX::XMFLOAT3 color);

private:

	// ͨ�� Drawable �̳�
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

};
