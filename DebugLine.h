#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"

class DebugLine:public Drawable
{
public:
	DebugLine(Graphics& gfx, DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3 color);
private:
	// ͨ�� Drawable �̳�
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};

class DottedLine :public Drawable 
{
public:
	DottedLine(Graphics& gfx, DirectX::XMFLOAT3 begin, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3 color);
private:
	// ͨ�� Drawable �̳�
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};