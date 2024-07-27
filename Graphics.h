#pragma once
#include <d3d11.h>
#include <sstream>
#include <vector>
#include <wrl.h>
#include <d3dcompiler.h>
#include "ChiliWin.h"

#include "ChiliException.h"

class Graphics {
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics()=default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue)noexcept;
	void DrawTestTriangle();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
};