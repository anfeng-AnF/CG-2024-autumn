#pragma once
#include "Bindable.h"
#include "BindableCommon.h"
namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		VertexShader(Graphics& gfx, const std::string& path, std::shared_ptr<ComputeShader> pComputeShader = nullptr);

		void Bind(Graphics& gfx) noexcept override;

		ID3DBlob* GetBytecode() const noexcept;
		static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, const std::string& path, std::shared_ptr<ComputeShader> pComputeShader = nullptr);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

		std::shared_ptr<ComputeShader> pComputeShader;
	};
}