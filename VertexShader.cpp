#include "VertexShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include <typeinfo>

namespace Bind
{
	VertexShader::VertexShader(Graphics& gfx, const std::string& path, std::shared_ptr<ComputeShader> _pComputeShader)
		:
		path(path),
		pComputeShader(_pComputeShader)
	{
		D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &pBytecodeBlob);
		GetDevice(gfx)->CreateVertexShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader
		);
	}

	void VertexShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		if (pComputeShader)
		{
			ID3D11ShaderResourceView* pSRV = pComputeShader->GetOutputSRV(gfx);
			if (pSRV)
			{
				GetContext(gfx)->VSSetShaderResources(0, 1, &pSRV);
			}
		}
	}

	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return pBytecodeBlob.Get();
	}
	std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, const std::string& path, std::shared_ptr<ComputeShader> _pComputeShader)
	{
		return Codex::Resolve<VertexShader>(gfx, path,_pComputeShader);
	}
	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}
	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID(path);
	}
}
