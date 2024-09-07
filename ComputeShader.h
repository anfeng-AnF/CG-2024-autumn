#pragma once
#include "Bindable.h"
#include <thread>
namespace Bind
{
	extern DWORD mainThreadId;
	class ComputeShader : public Bindable
	{
	public:
		ComputeShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		ID3D11ShaderResourceView* GetOutputSRV(Graphics& gfx) noexcept;
		void SetOutputBuffer(Graphics& gfx, ID3D11UnorderedAccessView* pUAV);

		static std::shared_ptr<ComputeShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;

	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pOutputSRV;

		Microsoft::WRL::ComPtr<ID3D11Query> pQuery;
		bool queryReady = false;
	};
}
