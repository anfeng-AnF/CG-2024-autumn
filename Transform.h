#pragma once

#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

class FTransform {
public:
    // 默认构造函数
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
    // 获取变换矩阵
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
    XMVECTOR position; // 位置
    XMVECTOR rotation; // 旋转（四元数）
    XMVECTOR scale;    // 缩放
};

