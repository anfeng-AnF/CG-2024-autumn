#include "ComputeShader.h"
#include <thread>
#include <cassert>

namespace Bind
{
    DWORD mainThreadId = GetCurrentThreadId();

    ComputeShader::ComputeShader(Graphics& gfx, const std::string& path)
        : path(path), queryReady(false)
    {
        D3DReadFileToBlob(std::wstring(path.begin(), path.end()).c_str(), &pBytecodeBlob);
        GetDevice(gfx)->CreateComputeShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, &pComputeShader);

        D3D11_QUERY_DESC queryDesc = {};
        queryDesc.Query = D3D11_QUERY_EVENT;
        queryDesc.MiscFlags = 0;
        GetDevice(gfx)->CreateQuery(&queryDesc, &pQuery);
    }

    void ComputeShader::Bind(Graphics& gfx) noexcept
    {
        GetContext(gfx)->CSSetShader(pComputeShader.Get(), nullptr, 0u);

        if (pUAV) {
            GetContext(gfx)->CSSetUnorderedAccessViews(0, 1, pUAV.GetAddressOf(), nullptr);
        }
    }

    void ComputeShader::SetOutputBuffer(Graphics& gfx, ID3D11UnorderedAccessView* pUAV)
    {
        Bind(gfx);

        GetContext(gfx)->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);

        GetContext(gfx)->Dispatch(1, 1, 1);

        queryReady = false;
        GetContext(gfx)->End(pQuery.Get());
    }

    ID3D11ShaderResourceView* ComputeShader::GetOutputSRV(Graphics& gfx) noexcept
    {
        //assert(GetCurrentThreadId() != mainThreadId && "Cannot call GetOutputSRV on the main thread; it may affect rendering performance");

        if (!queryReady)
        {
            while (GetContext(gfx)->GetData(pQuery.Get(), nullptr, 0, 0) == S_FALSE)
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            queryReady = true;
        }

        return pOutputSRV.Get();
    }

    std::shared_ptr<ComputeShader> ComputeShader::Resolve(Graphics& gfx, const std::string& path)
    {
        return std::make_shared<ComputeShader>(gfx, path);
    }

    std::string ComputeShader::GenerateUID(const std::string& path)
    {
        return typeid(ComputeShader).name() + '#' + path;
    }

    std::string ComputeShader::GetUID() const noexcept
    {
        return GenerateUID(path);
    }
}
