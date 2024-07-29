#include "Graphics.h"
//#include "GraphicsThrowMacros.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd, int width, int height)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = (float)width;
	sd.BufferDesc.Height = (float)height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
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

	// for checking results of d3d functions
	HRESULT hr;

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
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

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
	descDepth.Width = (float)width;
	descDepth.Height = (float)height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
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
}

void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(1u, 0u)))
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

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}


void Graphics::DrawTestTriangle(float angle)
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex
	{
		struct {
			float x;
			float y;
		}pos;
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};

	// create vertex buffer (1 2d triangle at center of screen)
	const Vertex vertices[] =
	{
		{{0.0f, 0.5f}, {255,255, 0, 0}},
		{{0.5f, -0.5f}, {0, 255, 0, 0}},
		{{-0.5f, -0.5f}, {0, 0, 255, 0}},
		{{-0.3f, 0.3f}, {0, 255, 0, 0}},
		{{0.3f, 0.3f}, {0, 0, 255, 0}},
		{{0.0f, -1.0f}, {255, 0, 0, 0}}
	};
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);

	//create index buffer
	const unsigned short indices[] = {
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

	//bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	struct ConstantBuffer {
		struct {
			float element[4][4];
		}transformation;
	};
	ConstantBuffer cb{
		{
		(640.0f / 1080) * cos(angle),-sin(angle),0,0,
		(640.0f / 1080) * sin(angle),cos(angle),0,0,
		0,0,1,0,
		0,0,0,1,
		}
	};
	wrl::ComPtr<ID3D11Buffer>pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	HRESULT hr1= pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	if (FAILED(hr1)) {

	}



	//bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


	wrl::ComPtr<ID3DBlob> pBlob;
	//create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

	//bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	// bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	//input vertex layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	);

	//bind input vertex layout
	pContext->IASetInputLayout(pInputLayout.Get());

	//bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	//set Primitive topology to triangle list
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 1080;
	vp.Height = 640;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	pContext->DrawIndexed((UINT)std::size(indices),0u, 0u);
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

