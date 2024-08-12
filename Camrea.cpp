#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	// ��ʼ��λ�ã���Ϊ��ģ������õ���ʼλ��
	transform.position = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f); // ���磬Ĭ��λ���� Z �Ḻ����

	// ��ʼ����ת�����Ƕ�ת��Ϊ��Ԫ��
	// ���磬�趨Ĭ����ת����Ԫ����ʾ�ĵ�λ��ת��û����ת��
	transform.rotation = DirectX::XMQuaternionIdentity(); // û����ת

	// ��ʼ�����ţ���Ϊ��ģ������õ���������
	transform.scale = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); // Ĭ����������Ϊ 1���������ţ�
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
	// ����Ԫ��ת��Ϊ����
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

	// ��ȡ�����Ԫ��
	float m11 = rotationMatrix.r[0].m128_f32[0];
	float m21 = rotationMatrix.r[1].m128_f32[0];
	float m31 = rotationMatrix.r[2].m128_f32[0];
	float m32 = rotationMatrix.r[2].m128_f32[1];
	float m33 = rotationMatrix.r[2].m128_f32[2];

	// ����ŷ����
	float pitch = atan2f(m32, m33);
	float yaw = asinf(-m31);
	float roll = atan2f(m21, m11);

	// ���Ƕ�ת��Ϊ�ȣ���ѡ��
	pitch *= (180.0f / XM_PI);
	yaw *= (180.0f / XM_PI);
	roll *= (180.0f / XM_PI);
	return { pitch,yaw,roll };
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{

		// ��ȡ��������
		float x = XMVectorGetX(transform.position);
		float y = XMVectorGetY(transform.position);
		float z = XMVectorGetZ(transform.position);

		// ʹ�� ImGui ��ʾλ��
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


