#pragma once
#include <d3d11.h>
#include <sstream>
#include <vector>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include "ChiliWin.h"

namespace Bind
{
	class Bindable;
}

class Graphics {
	friend Bind::Bindable;
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	void ClearBuffer(float red, float green, float blue)noexcept;
	void DrawIndexed(UINT count) noexcept;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

	void ReadBackBuffer(D3D11_MAPPED_SUBRESOURCE& msr);
	void WriteToBackBuffer(D3D11_MAPPED_SUBRESOURCE& msr);
	template<typename... Param>
	void PostProcessingOnCPU(void(*func)(D3D11_MAPPED_SUBRESOURCE&, int, int, Param...), Param&&... args) {
		D3D11_MAPPED_SUBRESOURCE resource;
		ReadBackBuffer(resource);
		D3D11_TEXTURE2D_DESC desc = {};
		pBackBuffer->GetDesc(&desc);
		func(resource, desc.Width,desc.Height,std::forward(args...));
		WriteToBackBuffer(resource);
	}

	template<typename Func, typename... Param>
	void PostProcessingOnCPU(Func func, Param&&... args) {
		D3D11_MAPPED_SUBRESOURCE resource;
		ReadBackBuffer(resource);
		D3D11_TEXTURE2D_DESC desc = {};
		pBackBuffer->GetDesc(&desc);
		func(resource, desc.Width, desc.Height,args...);
		WriteToBackBuffer(resource);
	}

	template<typename T, typename... Param>
	void PostProcessingOnCPU(T* obj, void (T::* func)(D3D11_MAPPED_SUBRESOURCE&, int, int, Param...), Param&&... args) {
		D3D11_MAPPED_SUBRESOURCE resource;
		ReadBackBuffer(resource);
		D3D11_TEXTURE2D_DESC desc = {};
		pBackBuffer->GetDesc(&desc);
		(obj->*func)(resource, desc.Width, desc.Height,std::forward(args...));
		WriteToBackBuffer(resource);
	}
//private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
};

