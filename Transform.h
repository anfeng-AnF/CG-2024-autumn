#pragma once

#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

class FTransform {
public:
    // Ĭ�Ϲ��캯��
    FTransform()
        : position(XMVectorZero()),
        rotation(XMQuaternionIdentity()),
        scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f))
    {}
    FTransform(DirectX::XMFLOAT3 _pos)
        : position({ _pos.x,_pos.y,_pos.z,0.0f }),
        rotation(XMQuaternionIdentity()),
        scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f))
    {}
    FTransform(DirectX::XMVECTOR _pos, DirectX::XMVECTOR _scale, DirectX::XMVECTOR _rotation):
        position(_pos),
        scale(_scale),
        rotation(_rotation)
    {}
    // ��ȡ�任����
    XMMATRIX GetMatrix() const {
        XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(position);
        return scaleMatrix * rotationMatrix * translationMatrix;
    }

    XMFLOAT3 GetRotationEuler()noexcept;
    XMVECTOR ComputeRotationQuaternion(const XMVECTOR& from, const XMVECTOR& to) noexcept;
    static const XMVECTOR ForwardVector, RightVector, UpVector;
public:
    XMVECTOR position; // λ��
    XMVECTOR rotation; // ��ת����Ԫ����
    XMVECTOR scale;    // ����
};

