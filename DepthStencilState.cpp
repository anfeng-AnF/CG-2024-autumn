#include "DepthStencilState.h"

Bind::DepthStencilState::DepthStencilState(Graphics& gfx)
{
	// ����һ����Ȳ��Խ��õ����״̬����
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE; // ������Ȳ���
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // ��ֹ���д��
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
