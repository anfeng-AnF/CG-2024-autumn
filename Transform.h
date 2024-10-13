#pragma once

#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;
bool IsPerspectiveMatrix(const XMMATRIX& m);

class FTransform {
public:
    // 默认构造函数
    FTransform();
    FTransform(DirectX::XMFLOAT3 _pos);
    FTransform(DirectX::XMVECTOR _pos, DirectX::XMVECTOR _scale, DirectX::XMVECTOR _rotation);
    FTransform(const XMMATRIX& matrix);

    // 获取变换矩阵
    XMMATRIX GetMatrix() const;
    XMMATRIX GetTranslateMatrix() const;
    XMMATRIX GetScaleMatrix() const;
    XMMATRIX GetRotationMatrix()const;

    FTransform operator+(const FTransform& other) const;

    XMFLOAT3 GetRotationEuler()noexcept;
    XMVECTOR ComputeRotationQuaternion(const XMVECTOR& from, const XMVECTOR& to) noexcept;

    XMVECTOR GetForwardVector() const;
    XMVECTOR GetRightVector() const;
    XMVECTOR GetUpVector() const;
    friend bool operator==(const FTransform& lhs, const FTransform& rhs);
public:
    static const XMVECTOR ForwardVector, RightVector, UpVector;
    XMVECTOR position; // 位置
    XMVECTOR rotation; // 旋转（四元数）
    XMVECTOR scale;    // 缩放
    static const XMVECTOR ForwardVector, RightVector, UpVector;
};

XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator*(const XMFLOAT3& lhs, float scalar);

XMFLOAT3 operator/(const XMFLOAT3& lhs, float scalar);

bool ArePointsEqual(const DirectX::XMFLOAT3& point1, const DirectX::XMFLOAT3& point2, float epsilon = 1e-6f);

XMVECTOR FindBetweenNormals(const XMVECTOR& quatA, const XMVECTOR& quatB);

bool IsQuaternionNaN(const XMVECTOR& quat);

float CalculateAngleBetweenVectors(DirectX::XMVECTOR vectorA, DirectX::XMVECTOR vectorB);

bool IsVectorNearZero(DirectX::XMVECTOR vector, float tolerance = 1e-6f);

XMMATRIX XMMatrixLerp(const XMMATRIX& A, const XMMATRIX& B, float t);
