#include "Transform.h"

XMFLOAT3 FTransform::GetRotationEuler() noexcept
{
	// 从四元数提取分量
	float w = XMVectorGetW(rotation);
	float x = XMVectorGetX(rotation);
	float y = XMVectorGetY(rotation);
	float z = XMVectorGetZ(rotation);

	// 计算 Roll, Pitch 和 Yaw
	float sinr_cosp = 2 * (w * x + y * z);
	float cosr_cosp = 1 - 2 * (x * x + y * y);
	float roll = std::atan2(sinr_cosp, cosr_cosp); // Roll

	float sinp = 2 * (w * y - z * x);
	float pitch;
	if (std::abs(sinp) >= 1)
		pitch = std::copysign(XM_PI / 2, sinp); // Use 90 degrees if out of range
	else
		pitch = std::asin(sinp); // Pitch

	float siny_cosp = 2 * (w * z + x * y);
	float cosy_cosp = 1 - 2 * (y * y + z * z);
	float yaw = std::atan2(siny_cosp, cosy_cosp); // Yaw

	return XMFLOAT3(roll, pitch, yaw);
}

XMVECTOR FTransform::ComputeRotationQuaternion(const XMVECTOR& from, const XMVECTOR& to) noexcept
{
	// 归一化两个向量
	XMVECTOR fromNormalized = XMVector3Normalize(from);
	XMVECTOR toNormalized = XMVector3Normalize(to);

	// 计算旋转轴（叉积）
	XMVECTOR axis = XMVector3Cross(fromNormalized, toNormalized);

	// 计算旋转角度（点积）
	float dotProduct = XMVectorGetX(XMVector3Dot(fromNormalized, toNormalized));
	float angle = acosf(dotProduct);

	// 创建旋转四元数
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(XMVector3Normalize(axis), angle);

	return rotationQuaternion;
}


// 静态常量变量的定义和初始化
const DirectX::XMVECTOR FTransform::ForwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR FTransform::RightVector = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR FTransform::UpVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);