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

    // ��ȡ�任����
    XMMATRIX GetMatrix() const {
        // �������ž���
        XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);

        // ������ת����
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);

        // ����ƽ�ƾ���
        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(position);

        // ��Ͼ���
        return scaleMatrix * rotationMatrix * translationMatrix;
    }

    XMFLOAT3 GetRotationEuler()noexcept;
    XMVECTOR ComputeRotationQuaternion(const XMVECTOR& from, const XMVECTOR& to) noexcept;

public:
    XMVECTOR position; // λ��
    XMVECTOR rotation; // ��ת����Ԫ����
    XMVECTOR scale;    // ����
};

