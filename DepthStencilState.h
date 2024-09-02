#pragma once
#include "Bindable.h"
namespace Bind
{
	class DepthStencilState : public Bindable
	{
	public:
		DepthStencilState(Graphics& gfx);
		void Bind(Graphics& gfx) noexcept override;
		void UnBind(Graphics& gfx)noexcept;
	protected:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	};
}