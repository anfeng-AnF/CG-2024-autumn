#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		void Update(Graphics& gfx, const C& consts)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext(gfx)->Map(
				pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			);
			memcpy(msr.pData, &consts, sizeof(consts));
			GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
		}
		ConstantBuffer(Graphics& gfx, const C& consts, UINT slot = 0u)
			:
			slot(slot)
		{


			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer);
		}
		ConstantBuffer(Graphics& gfx, UINT slot = 0u)
			:
			slot(slot)
		{


			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer);
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(this->slot, 1u, pConstantBuffer.GetAddressOf());
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot = 0)
		{
			using namespace std::string_literals;
			return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(this->slot, 1u, pConstantBuffer.GetAddressOf());
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot = 0)
		{
			using namespace std::string_literals;
			static int usage = 0;
			//simple way to fix different object use one constantBuffer
			return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot)+std::to_string(usage++);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};
}