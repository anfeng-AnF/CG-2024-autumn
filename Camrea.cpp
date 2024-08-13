#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	Reset();
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

		XMFLOAT3 euler = transform.GetRotationEuler();
		ImGui::Text("Rotation (Pitch,Roll,Yaw): %.2f, %.2f, %.2f", euler.x, euler.y, euler.z);


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
	transform.position = XMVectorSet(-6.0f, 10.0f, 8.0f, 0.0f);
	transform.rotation = { 0.846903, 0.221031, 0.123545, 0.467586 };
	EnableRollRotate = false;
}

void Camera::RotatePitchYaw(float dx, float dy) noexcept
{
	XMVECTOR UpVector;
	XMVECTOR RightVector = XMVector3TransformNormal(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		XMMatrixRotationQuaternion(transform.rotation));

	if (EnableRollRotate) {
		UpVector = XMVector3TransformNormal(
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			XMMatrixRotationQuaternion(transform.rotation));
	}
	else
	{
		UpVector = { 0.0f, 0.0F, 1.0f, 0.0f };
		//RightVector = -_mm_and_ps(RightVector, { 1.0f, 0.0f, 1.0f, 0.0f });
	}

	XMVECTOR deltaYawRotation = XMQuaternionRotationNormal(UpVector, dx * rotationSpeed);
	XMVECTOR deltaPitchRotation = XMQuaternionRotationNormal(RightVector, dy * rotationSpeed);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaYawRotation);
	transform.rotation = DirectX::XMQuaternionNormalize(transform.rotation);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaPitchRotation);
	transform.rotation = DirectX::XMQuaternionNormalize(transform.rotation);
}

void Camera::RotateRoll(float dz) noexcept
{
	EnableRollRotate = true;
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

	XMVECTOR UpVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	if (EnableRollRotate) {
		UpVector = XMVector3TransformNormal(
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			XMMatrixRotationQuaternion(transform.rotation));
	}

	transform.position +=
		DirectX::XMVectorScale(ForwardVector, deltaTranslation.x * travelSpeed)
		+ DirectX::XMVectorScale(RightVector, deltaTranslation.y * travelSpeed)
		+ DirectX::XMVectorScale(UpVector, deltaTranslation.z * travelSpeed);
}

void Camera::addTravelSpeed()
{
	if (travelSpeed >= 20) {
		travelSpeed = 20;
		return;
	}
	travelSpeed += travelSpeed >= 1 ? 1 : 0.1;
}

void Camera::DecreaseTravelSpeed()
{

	if (travelSpeed <= 0.1)
	{
		travelSpeed = 0.1;
		return;
	}
	travelSpeed -= travelSpeed >= 1 ? 1 : 0.1;
}





