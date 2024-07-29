#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <Windows.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")

std::string HRESULTToString(HRESULT hr)
{
    // 用于存储错误消息
    char* buffer = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buffer,
        0,
        nullptr);

    std::string message = buffer;
    LocalFree(buffer);

    return message;
}

void CheckHRESULT(HRESULT hr, const std::string& context)
{
    if (FAILED(hr))
    {
        std::string errorMsg = context + ": " + HRESULTToString(hr);
        throw std::runtime_error(errorMsg);
    }
}
void CheckD3DErrors(ID3D11InfoQueue* infoQueue)
{
    UINT64 numMessages = infoQueue->GetNumStoredMessages();
    for (UINT64 i = 0; i < numMessages; ++i)
    {
        SIZE_T messageLength = 0;
        infoQueue->GetMessage(i, nullptr, &messageLength);
        std::vector<char> messageData(messageLength);
        D3D11_MESSAGE* message = reinterpret_cast<D3D11_MESSAGE*>(messageData.data());
        infoQueue->GetMessage(i, message, &messageLength);

        std::string errorMsg = "Direct3D11 Error: ";
        errorMsg += message->pDescription;

        throw std::runtime_error(errorMsg);
    }
}

void InitializeD3D11(HWND hwnd)
{
    HRESULT hr;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11InfoQueue* infoQueue = nullptr;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = 800;
    swapChainDesc.BufferDesc.Height = 600;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain,
        &device,
        &featureLevel,
        &context);

    CheckHRESULT(hr, "Failed to create device and swap chain");

    if (SUCCEEDED(device->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&infoQueue))))
    {
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    }

    try
    {
        // Example draw call
        context->DrawIndexed(36, 0, 0); // Replace with actual parameters

        // Check for errors after the draw call
        if (infoQueue)
        {
            CheckD3DErrors(infoQueue);
        }
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Direct3D Error", MB_OK | MB_ICONERROR);
        throw;
    }

    // Release resources
    if (infoQueue) infoQueue->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
}

int WINAPI WinMain1(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Create a simple window for testing
    WNDCLASS wc = { };
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"D3D11TestWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"D3D11 Test Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
    {
        MessageBoxA(nullptr, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);

    try
    {
        InitializeD3D11(hwnd);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Initialization failed", MB_OK | MB_ICONERROR);
        return -1;
    }

    MessageBoxA(nullptr, "Initialization succeeded.", "Info", MB_OK | MB_ICONINFORMATION);
    return 0;
}
