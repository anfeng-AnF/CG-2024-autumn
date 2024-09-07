#include "GeometryShader.h"

namespace Bind
{
	GeometryShader::GeometryShader(Graphics& gfx, const std::string& path)
		: path(path)
	{
		// Load the compiled shader from file
		HRESULT hr = D3DReadFileToBlob(std::wstring(path.begin(), path.end()).c_str(), &pBytecodeBlob);
		assert(SUCCEEDED(hr) && "Failed to load Geometry Shader bytecode");

		// Create the Geometry Shader
		hr = GetDevice(gfx)->CreateGeometryShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pGeometryShader
		);
		assert(SUCCEEDED(hr) && "Failed to create Geometry Shader");
	}

	void GeometryShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->GSSetShader(pGeometryShader.Get(), nullptr, 0u);
	}

	ID3DBlob* GeometryShader::GetBytecode() const noexcept
	{
		return pBytecodeBlob.Get();
	}

	std::shared_ptr<GeometryShader> GeometryShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return std::make_shared<GeometryShader>(gfx, path);
	}

	std::string GeometryShader::GenerateUID(const std::string& path)
	{
		return typeid(GeometryShader).name() + '#' + path;
	}

	std::string GeometryShader::GetUID() const noexcept
	{
		return GenerateUID(path);
	}
}
