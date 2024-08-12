#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class FTransform {
public:
    // 默认构造函数
    FTransform()
        : position(XMVectorZero()),
        rotation(XMQuaternionIdentity()),
        scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f))
    {}

    // 获取变换矩阵
    XMMATRIX GetMatrix() const {
        // 计算缩放矩阵
        XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);

        // 计算旋转矩阵
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);

        // 计算平移矩阵
        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(position);

        // 组合矩阵
        return scaleMatrix * rotationMatrix * translationMatrix;
    }

public:
    XMVECTOR position; // 位置
    XMVECTOR rotation; // 旋转（四元数）
    XMVECTOR scale;    // 缩放
};

