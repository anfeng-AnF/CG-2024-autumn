#include "Transform.h"
#include <algorithm>

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

XMMATRIX FTransform::GetTranslateMatrix() const
{
	return XMMatrixTranslationFromVector(position);
}

XMMATRIX FTransform::GetScaleMatrix() const
{
	return XMMatrixScalingFromVector(scale);
}

XMMATRIX FTransform::GetRotationMatrix() const
{
	return XMMatrixRotationQuaternion(rotation);
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

XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMFLOAT3(
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	);
}

XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMFLOAT3(
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z
	);
}

XMFLOAT3 operator*(const XMFLOAT3& lhs, float scalar)
{
	return XMFLOAT3(
		lhs.x * scalar,
		lhs.y * scalar,
		lhs.z * scalar
	);
}

XMFLOAT3 operator/(const XMFLOAT3& lhs, float scalar)
{
	return XMFLOAT3(
		lhs.x / scalar,
		lhs.y / scalar,
		lhs.z / scalar
	);
}

bool ArePointsEqual(const DirectX::XMFLOAT3& point1, const DirectX::XMFLOAT3& point2, float epsilon)
{
	using namespace DirectX;

	XMVECTOR vec1 = XMLoadFloat3(&point1);
	XMVECTOR vec2 = XMLoadFloat3(&point2);

	XMVECTOR diff = XMVectorSubtract(vec1, vec2);

	float distance = XMVectorGetX(XMVector3Length(diff));

	return distance < epsilon;
}

XMVECTOR FindBetweenNormals(const XMVECTOR& A, const XMVECTOR& B) {
	float W = 1.f + XMVectorGetX(XMVector4Dot(A, B));
	XMVECTOR Result;

	if (W >= 1e-6f * 1.f)
	{
		//Result = FVector::CrossProduct(A, B);
		Result = {
			XMVectorGetY(A)* XMVectorGetZ(B) - XMVectorGetZ(A) * XMVectorGetY(B),
			XMVectorGetZ(A)* XMVectorGetX(B) - XMVectorGetX(A) * XMVectorGetZ(B),
			XMVectorGetX(A)* XMVectorGetY(B) - XMVectorGetY(A) * XMVectorGetX(B),
			W };
	}
	else
	{
		// A and B point in opposite directions
		W = 0.f;
		const float X = std::abs(XMVectorGetY(A));
		const float Y = std::abs(XMVectorGetZ(A));
		const float Z = std::abs(XMVectorGetX(A));

		// Find orthogonal basis. 
		const XMVECTOR Basis = (X > Y && X > Z) ? XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) : XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		//Result = FVector::CrossProduct(A, Basis);
		Result = {
			XMVectorGetY(A) * XMVectorGetZ(Basis) - XMVectorGetZ(A) * XMVectorGetY(Basis),
			XMVectorGetZ(A) * XMVectorGetX(Basis) - XMVectorGetX(A) * XMVectorGetZ(Basis),
			XMVectorGetX(A) * XMVectorGetY(Basis) - XMVectorGetY(A) * XMVectorGetX(Basis),
			W };
	}

	return XMQuaternionNormalize(Result);
}

bool IsQuaternionNaN(const XMVECTOR& quat)
{
	return std::isnan(XMVectorGetX(quat)) ||
		std::isnan(XMVectorGetY(quat)) ||
		std::isnan(XMVectorGetZ(quat)) ||
		std::isnan(XMVectorGetW(quat));
}

float CalculateAngleBetweenVectors(DirectX::XMVECTOR vectorA, DirectX::XMVECTOR vectorB)

{
	vectorA = DirectX::XMVector3Normalize(vectorA);
	vectorB = DirectX::XMVector3Normalize(vectorB);

	float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(vectorA, vectorB));

	dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);

	float angle = acosf(dotProduct);

	return angle; // 弧度值
}

bool IsVectorNearZero(DirectX::XMVECTOR vector, float tolerance)

{
	DirectX::XMVECTOR zeroVector = DirectX::XMVectorZero();
	DirectX::XMVECTOR epsilon = DirectX::XMVectorReplicate(tolerance);

	return DirectX::XMVector3NearEqual(vector, zeroVector, epsilon);
}