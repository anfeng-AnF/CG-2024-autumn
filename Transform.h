#pragma once

#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

class FTransform {
public:
    // Ĭ�Ϲ��캯��
    FTransform();
    FTransform(DirectX::XMFLOAT3 _pos);
    FTransform(DirectX::XMVECTOR _pos, DirectX::XMVECTOR _scale, DirectX::XMVECTOR _rotation);
    FTransform(const XMMATRIX& matrix);

    // ��ȡ�任����
    XMMATRIX GetMatrix() const;

    FTransform operator+(const FTransform& other) const;

    XMFLOAT3 GetRotationEuler()noexcept;
    XMVECTOR ComputeRotationQuaternion(const XMVECTOR& from, const XMVECTOR& to) noexcept;

    XMVECTOR GetForwardVector() const;
    XMVECTOR GetRightVector() const;
    XMVECTOR GetUpVector() const;

public:
    static const XMVECTOR ForwardVector, RightVector, UpVector;
    XMVECTOR position; // λ��
    XMVECTOR rotation; // ��ת����Ԫ����
    XMVECTOR scale;    // ����
    static const XMVECTOR ForwardVector, RightVector, UpVector;
};

