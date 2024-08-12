#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	// 初始化位置：设为建模软件常用的起始位置
	transform.position = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f); // 例如，默认位置在 Z 轴负方向

	// 初始化旋转：将角度转换为四元数
	// 例如，设定默认旋转（四元数表示的单位旋转，没有旋转）
	transform.rotation = DirectX::XMQuaternionIdentity(); // 没有旋转

	// 初始化缩放：设为建模软件常用的缩放因子
	transform.scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); // 默认缩放因子为 1（即不缩放）
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	XMVECTOR ForwardVector = XMVector3TransformNormal(
		{ 0.0f, 0.0f, 1.0f, 0.0f }, 
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR UpVector = XMVector3TransformNormal(
		{ 0.0f, 1.0f, 0.0f, 0.0f }, 
		XMMatrixRotationQuaternion(transform.rotation));


	return DirectX::XMMatrixLookAtLH(
		transform.position,
		XMVectorAdd(transform.position, ForwardVector),
		UpVector
	);
}
DirectX::XMFLOAT3 QuaternionToEuler(const XMVECTOR& quaternion) {
	// 将四元数转换为矩阵
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

	// 提取矩阵的元素
	float m11 = rotationMatrix.r[0].m128_f32[0];
	float m21 = rotationMatrix.r[1].m128_f32[0];
	float m31 = rotationMatrix.r[2].m128_f32[0];
	float m32 = rotationMatrix.r[2].m128_f32[1];
	float m33 = rotationMatrix.r[2].m128_f32[2];

	// 计算欧拉角
	float pitch = atan2f(m32, m33);
	float yaw = asinf(-m31);
	float roll = atan2f(m21, m11);

	// 将角度转换为度（可选）
	pitch *= (180.0f / XM_PI);
	yaw *= (180.0f / XM_PI);
	roll *= (180.0f / XM_PI);
	return { pitch,yaw,roll };
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{

		// 提取各个分量
		float x = XMVectorGetX(transform.position);
		float y = XMVectorGetY(transform.position);
		float z = XMVectorGetZ(transform.position);

		// 使用 ImGui 显示位置
		ImGui::Text("Position: %f, %f, %f", x, y, z);

		XMFLOAT3 euler = QuaternionToEuler(transform.rotation);
		ImGui::Text("Rotation (Pitch, Yaw, Roll): %.2f, %.2f, %.2f", euler.x, euler.y, euler.z);


		if (ImGui::Button("Reset"))
		{
			Reset();
		}
		ImGui::End();
	}
}

void Camera::Reset() noexcept
{
	transform = FTransform{};
}

void Camera::RotatePitchYaw(float dx, float dy) noexcept
{
	XMVECTOR RightVector = XMVector3TransformNormal(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR UpVector = XMVector3TransformNormal(
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		XMMatrixRotationQuaternion(transform.rotation));


	XMVECTOR deltaYawRotation = XMQuaternionRotationAxis(UpVector, dx * rotationSpeed);
	XMVECTOR deltaPitchRotation = XMQuaternionRotationAxis(RightVector, dy * rotationSpeed);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaYawRotation);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaPitchRotation);
	transform.rotation = DirectX::XMQuaternionNormalize(transform.rotation);
}

void Camera::RotateRoll(float dz) noexcept
{
	XMVECTOR ForwardVector = XMVector3TransformNormal(
		{ 0.0f, 0.0f, 1.0f, 0.0f },
	XMMatrixRotationQuaternion(transform.rotation));
	XMVECTOR deltaRollRotation = XMQuaternionRotationAxis(ForwardVector, dz * rotationSpeed);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaRollRotation);
	transform.rotation = DirectX::XMQuaternionNormalize(transform.rotation);
}

void Camera::Translate(DirectX::XMFLOAT3 deltaTranslation) noexcept
{
	XMVECTOR ForwardVector = XMVector3TransformNormal(
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR RightVector = XMVector3TransformNormal(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR UpVector = XMVector3TransformNormal(
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		XMMatrixRotationQuaternion(transform.rotation));

	transform.position +=
		DirectX::XMVectorScale(ForwardVector,deltaTranslation.x)
		+ DirectX::XMVectorScale(RightVector, deltaTranslation.y)
		+ DirectX::XMVectorScale(UpVector, deltaTranslation.z);
}


