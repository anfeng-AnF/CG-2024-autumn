#include "Graphics.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
//#include "GraphicsThrowMacros.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
namespace wrl = Microsoft::WRL;
#define SMAACount 1u;
Graphics::Graphics(HWND hWnd, int width, int height)
{
	UINT Quality = 0u;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = SMAACount;
	sd.SampleDesc.Quality = Quality;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create device and front/back buffers, and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	);

	// gain access to texture subresource in swap chain (back buffer)
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = sd.BufferDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	rtvDesc.Texture2D.MipSlice = 0;

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, &pTarget);

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	// bind depth state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create depth stensil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = SMAACount;
	descDepth.SampleDesc.Quality = Quality;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &pDSV
	);

	// bind depth stensil view to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::EndFrame()
{	
	// imgui frame end
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(0u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			//GFX_THROW_HR(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			//GFX_THROW_HR(hr);
		}
	}
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	// imgui begin frame
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}


void Graphics::DrawIndexed(UINT count) noexcept
{
	pContext->DrawIndexed(count, 0u, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

void Graphics::ReadBackBuffer(D3D11_MAPPED_SUBRESOURCE& msr)
{
	if (!pBackBuffer) {
		pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
	}

	D3D11_TEXTURE2D_DESC desc = {};
	pBackBuffer->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.SampleDesc.Count = SMAACount;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pStagingTexture;
	pDevice->CreateTexture2D(&desc, nullptr, &pStagingTexture);

	pContext->CopyResource(pStagingTexture.Get(), pBackBuffer.Get());

	pContext->Map(pStagingTexture.Get(), 0, D3D11_MAP_READ, 0, &msr);
}

void Graphics::WriteToBackBuffer(D3D11_MAPPED_SUBRESOURCE& msr)
{
	if (!pBackBuffer) {
		pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
	}

	D3D11_TEXTURE2D_DESC desc = {};
	pBackBuffer->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pProcessedTexture;
	pDevice->CreateTexture2D(&desc, nullptr, &pProcessedTexture);

	pContext->UpdateSubresource(pProcessedTexture.Get(), 0, nullptr, msr.pData, msr.RowPitch, 0);

	pContext->CopyResource(pBackBuffer.Get(), pProcessedTexture.Get());
}

