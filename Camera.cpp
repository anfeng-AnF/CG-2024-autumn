#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

const FTransform Camera::FrontView = FTransform{ {0.0f, 0.0f, -30.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f) }; // Front View
const FTransform Camera::RightView = FTransform{ {30.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, XMQuaternionRotationRollPitchYaw(0.0f, -XM_PI / 2, 0.0f) }; // Right View
const FTransform Camera::TopView   = FTransform{ {0.0f, 30.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, XMQuaternionRotationRollPitchYaw(XM_PI / 2, 0.0f, 0.0f) }; // Top View

Camera::Camera(Window& window) :inputState(window, this)
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	XMVECTOR ForwardVector = XMVector3TransformNormal(
		FTransform::ForwardVector, 
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR UpVector = XMVector3TransformNormal(
		FTransform::UpVector, 
		XMMatrixRotationQuaternion(transform.rotation));


	return DirectX::XMMatrixLookAtLH(
		transform.position,
		XMVectorAdd(transform.position, ForwardVector),
		UpVector
	);
}


void Camera::SpawnControlWindow() noexcept
{
	ImGui::Begin("Camera");

	// 提取各个分量
	float x = XMVectorGetX(transform.position);
	float y = XMVectorGetY(transform.position);
	float z = XMVectorGetZ(transform.position);

	// 使用 ImGui 显示位置
	ImGui::Text("Position: %f, %f, %f", x, y, z);
	XMFLOAT3 euler = transform.GetRotationEuler();
	ImGui::Text("Rotation (Pitch,Roll,Yaw): %.2f, %.2f, %.2f", euler.x, euler.y, euler.z);
	ImGui::Text("Travel Speed: %f", travelSpeed);

	if (ImGui::Button("Reset"))
	{
		Reset();
	}

	ImGui::End();
}

void Camera::Reset() noexcept
{
	transform = FTransform{};
	transform.position = XMVectorSet(-10.0f,13.0f,-16.0f, 0.0f);
	transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(0.5f,0.52f,0.0f);
	EnableRollRotate = false;
}

void Camera::SetTransform(FTransform transform)
{
	this->transform = transform;
}

void Camera::RotatePitchYaw(float dx, float dy) noexcept
{
	XMVECTOR UpVector = XMVector3TransformNormal(
		FTransform::UpVector,
		XMMatrixRotationQuaternion(transform.rotation));
	XMVECTOR RightVector = XMVector3TransformNormal(
		FTransform::RightVector,
		XMMatrixRotationQuaternion(transform.rotation));

	if (!EnableRollRotate) {
		UpVector = {0.0f,XMVectorGetY(UpVector),0.0f,0.0f};
	}


	XMVECTOR deltaYawRotation = XMQuaternionRotationNormal(UpVector, dx * rotationSpeed);
	XMVECTOR deltaPitchRotation = XMQuaternionRotationNormal(RightVector, dy * rotationSpeed);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaPitchRotation);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaYawRotation);
	transform.rotation = DirectX::XMQuaternionNormalize(transform.rotation);
}

void Camera::RotateRoll(float dz) noexcept
{
	EnableRollRotate = true;

	XMVECTOR ForwardVector = XMVector3TransformNormal(
		FTransform::ForwardVector,
	XMMatrixRotationQuaternion(transform.rotation));
	XMVECTOR deltaRollRotation = XMQuaternionRotationAxis(ForwardVector, dz * rotationSpeed);
	transform.rotation = DirectX::XMQuaternionMultiply(transform.rotation, deltaRollRotation);
	transform.rotation = DirectX::XMQuaternionNormalize(transform.rotation);
}

void Camera::Translate(DirectX::XMFLOAT3 deltaTranslation) noexcept
{
	XMVECTOR ForwardVector = XMVector3TransformNormal(
		FTransform::ForwardVector,
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR RightVector = XMVector3TransformNormal(
		FTransform::RightVector,
		XMMatrixRotationQuaternion(transform.rotation));

	XMVECTOR UpVector = FTransform::UpVector;
	if (EnableRollRotate) {
		UpVector = XMVector3TransformNormal(
			FTransform::UpVector,
			XMMatrixRotationQuaternion(transform.rotation));
	}

	transform.position +=
		DirectX::XMVectorScale(ForwardVector, deltaTranslation.x * travelSpeed)
		+ DirectX::XMVectorScale(RightVector, deltaTranslation.y * travelSpeed)
		+ DirectX::XMVectorScale(UpVector, deltaTranslation.z * travelSpeed);
}

void Camera::IncreaseTravelSpeed()
{
	if (travelSpeed >= 20.0f) {
		travelSpeed = 20.0f;
		return;
	}
	travelSpeed += travelSpeed >= 1.0f ? 1.0f : 0.1f;
}

void Camera::DecreaseTravelSpeed()
{
	if (travelSpeed <= 0.15f)
	{
		travelSpeed = 0.1f;
		return;
	}
	travelSpeed -= travelSpeed > 1.0f ? 1.0f : 0.1f;
}

FTransform Camera::GetTransform()const
{
	return transform;
}

XMFLOAT3 Camera::GetForwardVector()const noexcept
{
	XMFLOAT3 forwadrVector;
    DirectX::XMStoreFloat3(&forwadrVector,XMVector3TransformNormal(
		FTransform::ForwardVector,
		XMMatrixRotationQuaternion(transform.rotation)));
	return forwadrVector;
}

XMFLOAT3 Camera::GetRightVector() const noexcept
{
	XMFLOAT3 rightVector;
	DirectX::XMStoreFloat3(&rightVector,XMVector3TransformNormal(
		FTransform::RightVector,
		XMMatrixRotationQuaternion(transform.rotation)));
	return rightVector;
}

XMFLOAT3 Camera::GetUpVector() const noexcept
{
	XMFLOAT3 UpVector;
	DirectX::XMStoreFloat3(&UpVector, XMVector3TransformNormal(
		FTransform::UpVector,
		XMMatrixRotationQuaternion(transform.rotation)));
	return UpVector;
}

void Camera::CameraMove::Enter()
{
	wnd.DisableCursor();
	wnd.mouse.Flush();
	wnd.mouse.FlushRawDelta();
	wnd.mouse.EnableRaw();
}

void Camera::CameraMove::Update(float deltaTime)
{
	//Handle keyboard input
	bool isPerspective = IsPerspectiveMatrix(wnd.Gfx().GetProjection());
	if (wnd.Kbd.KeyIsPressed('W') && isPerspective)
	{
		cam->Translate({ deltaTime,0.0f,0.0f });
	}
	if (wnd.Kbd.KeyIsPressed('A'))
	{
		cam->Translate({ 0.0f,-deltaTime,0.0f });
	}
	if (wnd.Kbd.KeyIsPressed('S') && isPerspective)
	{
		cam->Translate({ -deltaTime,0.0f,0.0f });
	}
	if (wnd.Kbd.KeyIsPressed('D'))
	{
		cam->Translate({ 0.0f,deltaTime,0.0f });
	}
	if (wnd.Kbd.KeyIsPressed(VK_SPACE)) {
		cam->Translate({ 0.0f,0.0f,deltaTime });
	}
	if (wnd.Kbd.KeyIsPressed(VK_CONTROL)) {
		cam->Translate({ 0.0f,0.0f,-deltaTime });
	}
	if (wnd.Kbd.KeyIsPressed('Q'))
	{
		cam->RotateRoll(1.0f);
	}
	if (wnd.Kbd.KeyIsPressed('E'))
	{
		cam->RotateRoll(-1.0f);
	}
	//Handling of mouse input
	if (wnd.mouse.RightIsPressed())
	{
		while (const auto delta = wnd.mouse.ReadRawDelta())
		{
			cam->RotatePitchYaw((float)delta->x, (float)delta->y);
		}
	}
	else
	{
		this->Machine->SetState(PERVIOUS_STATE);
	}
	while (const auto delta = wnd.mouse.Read()) {
		switch (delta.value().GetType())
		{
		case Mouse::Event::Type::WheelDown:
			cam->DecreaseTravelSpeed();
			break;
		case Mouse::Event::Type::WheelUp:
			cam->IncreaseTravelSpeed();
			break;
		default:
			break;
		}
	}
}

void Camera::CameraMove::Exit()
{
	wnd.EnableCursor();
	wnd.mouse.DisableRaw();
}
