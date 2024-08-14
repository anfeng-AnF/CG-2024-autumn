#include "Transform.h"

XMFLOAT3 FTransform::GetRotationEuler() noexcept
{
	// ����Ԫ����ȡ����
	float w = XMVectorGetW(rotation);
	float x = XMVectorGetX(rotation);
	float y = XMVectorGetY(rotation);
	float z = XMVectorGetZ(rotation);

	// ���� Roll, Pitch �� Yaw
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
	// ��һ����������
	XMVECTOR fromNormalized = XMVector3Normalize(from);
	XMVECTOR toNormalized = XMVector3Normalize(to);

	// ������ת�ᣨ�����
	XMVECTOR axis = XMVector3Cross(fromNormalized, toNormalized);

	// ������ת�Ƕȣ������
	float dotProduct = XMVectorGetX(XMVector3Dot(fromNormalized, toNormalized));
	float angle = acosf(dotProduct);

	// ������ת��Ԫ��
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(XMVector3Normalize(axis), angle);

	return rotationQuaternion;
}


// ��̬���������Ķ���ͳ�ʼ��
const DirectX::XMVECTOR FTransform::ForwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR FTransform::RightVector = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR FTransform::UpVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);