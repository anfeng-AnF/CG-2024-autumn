#pragma once

#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

class FTransform {
public:
    FTransform();
    FTransform(DirectX::XMFLOAT3 _pos);
    FTransform(DirectX::XMVECTOR _pos, DirectX::XMVECTOR _scale, DirectX::XMVECTOR _rotation);
    FTransform(const XMMATRIX& matrix);

    // Get the transformation matrix
    XMMATRIX GetMatrix() const;

    FTransform operator+(const FTransform& other) const;

    XMFLOAT3 GetRotationEuler()noexcept;
    XMVECTOR ComputeRotationQuaternion(const XMVECTOR& from, const XMVECTOR& to) noexcept;

    XMVECTOR GetForwardVector() const;
    XMVECTOR GetRightVector() const;
    XMVECTOR GetUpVector() const;

public:
    XMVECTOR position; // λ��
    XMVECTOR rotation; // ��ת����Ԫ����
    XMVECTOR scale;    // ����
    static const XMVECTOR ForwardVector, RightVector, UpVector;
};

