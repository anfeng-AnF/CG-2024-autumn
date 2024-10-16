#pragma once
#include "Bindable.h"

namespace Bind
{
	class GeometryShader : public Bindable
	{
	public:
		GeometryShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		void UnBind(Graphics& gfx)noexcept;
		ID3DBlob* GetBytecode() const noexcept;
		static std::shared_ptr<GeometryShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
	};
}