#include "Transform.h"

FTransform::FTransform()
	: position(XMVectorZero()),
	rotation(XMQuaternionIdentity()),
	scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f))
{}

FTransform::FTransform(DirectX::XMFLOAT3 _pos)
	: position({ _pos.x,_pos.y,_pos.z,0.0f }),
	rotation(XMQuaternionIdentity()),
	scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f))
{}

FTransform::FTransform(DirectX::XMVECTOR _pos, DirectX::XMVECTOR _scale, DirectX::XMVECTOR _rotation)
	:
	position(_pos),
	scale(_scale),
	rotation(_rotation)
{}

FTransform::FTransform(const XMMATRIX & matrix)
{
	// 提取缩放、旋转和位置
	XMVECTOR scaling, rotationQuat, translation;
	XMMatrixDecompose(&scaling, &rotationQuat, &translation, matrix);

	position = translation;
	scale = scaling;
	rotation = rotationQuat;
}

XMMATRIX FTransform::GetMatrix() const {
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(position);
	return scaleMatrix * rotationMatrix * translationMatrix;
}

FTransform FTransform::operator+(const FTransform& other) const
{
	XMVECTOR combinedScale = XMVectorMultiply(scale, other.scale);
	XMVECTOR combinedRotation = XMQuaternionMultiply(rotation, other.rotation);
	XMVECTOR combinedPosition = position + other.position;
	return FTransform(combinedPosition, combinedScale, combinedRotation);
}

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
	XMVECTOR fromNormalized = XMVector3Normalize(from);
	XMVECTOR toNormalized = XMVector3Normalize(to);

	XMVECTOR axis = XMVector3Cross(fromNormalized, toNormalized);
	float dotProduct = XMVectorGetX(XMVector3Dot(fromNormalized, toNormalized));
	float angle = acosf(dotProduct);
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(XMVector3Normalize(axis), angle);
	return rotationQuaternion;
}

XMVECTOR FTransform::GetForwardVector() const {
	return XMVector3Rotate(ForwardVector, rotation);
}

XMVECTOR FTransform::GetRightVector() const {
	return XMVector3Rotate(RightVector, rotation);
}

XMVECTOR FTransform::GetUpVector() const {
	return XMVector3Rotate(UpVector, rotation);
}


//These direction vectors are the default world coordinate direction vectors in the directx coordinate system, oriented in the z+ direction
const DirectX::XMVECTOR FTransform::ForwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR FTransform::RightVector = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR FTransform::UpVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

bool IsPerspectiveMatrix(const XMMATRIX& m)
{
	return (m.r[3].m128_f32[3] == 0.0f && m.r[3].m128_f32[2] != 0.0f);
}

bool operator==(const FTransform& lhs, const FTransform& rhs)
{
	
	return DirectX::XMVector3Equal(lhs.position, rhs.position) &&
		DirectX::XMQuaternionEqual(lhs.rotation, rhs.rotation) &&
		DirectX::XMVector3Equal(lhs.scale, rhs.scale);
}
