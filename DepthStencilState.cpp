#include "DepthStencilState.h"

Bind::DepthStencilState::DepthStencilState(Graphics& gfx)
{
	// 创建一个深度测试禁用的深度状态描述
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE; // 禁用深度测试
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 禁止深度写入
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	GetDevice(gfx)->CreateDepthStencilState(&depthStencilDesc, pDepthStencilState.GetAddressOf());
}

void Bind::DepthStencilState::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->OMSetDepthStencilState(pDepthStencilState.Get(), 1);
}

void Bind::DepthStencilState::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->OMSetDepthStencilState(nullptr, 0);
}
