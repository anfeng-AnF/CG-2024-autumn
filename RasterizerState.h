#pragma once
#include "Bindable.h"
namespace Bind
{
	class RasterizerState : public Bindable
	{
	public:
		RasterizerState(Graphics& gfx, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, int depthBias = 0, float slopeScaledDepthBias = 0.0f);
		void Bind(Graphics& gfx) noexcept override;
		void UnBind(Graphics& gfx)noexcept;
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState;
	};
}

inline Bind::RasterizerState::RasterizerState(Graphics& gfx, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, int depthBias, float slopeScaledDepthBias)
{
	// 创建光栅化器状态描述
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = fillMode;
	rasterDesc.CullMode = cullMode;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = depthBias;
	rasterDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.AntialiasedLineEnable = FALSE;

	// 创建光栅化器状态对象
	HRESULT hr = GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizerState);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create rasterizer state");
	}
}

inline void Bind::RasterizerState::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->RSSetState(pRasterizerState.Get());
}

inline void Bind::RasterizerState::UnBind(Graphics& gfx) noexcept
{
	GetContext(gfx)->RSSetState(nullptr);
}
