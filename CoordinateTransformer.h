#include <DirectXMath.h>

namespace AnF {
    // 将DirectX坐标转换为Unreal Engine坐标
    DirectX::XMFLOAT3 ToUE(const DirectX::XMFLOAT3& dxCoord) noexcept {
        return DirectX::XMFLOAT3(dxCoord.z, dxCoord.y, -dxCoord.x);
    }

    // 将Unreal Engine坐标转换为DirectX坐标
    DirectX::XMFLOAT3 ToDX(const DirectX::XMFLOAT3& ueCoord) noexcept {
        return DirectX::XMFLOAT3(-ueCoord.z, ueCoord.y, ueCoord.x);
    }
}