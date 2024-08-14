#include <DirectXMath.h>

namespace AnF {
    // ��DirectX����ת��ΪUnreal Engine����
    DirectX::XMFLOAT3 ToUE(const DirectX::XMFLOAT3& dxCoord) noexcept {
        return DirectX::XMFLOAT3(dxCoord.z, dxCoord.y, -dxCoord.x);
    }

    // ��Unreal Engine����ת��ΪDirectX����
    DirectX::XMFLOAT3 ToDX(const DirectX::XMFLOAT3& ueCoord) noexcept {
        return DirectX::XMFLOAT3(-ueCoord.z, ueCoord.y, ueCoord.x);
    }
}