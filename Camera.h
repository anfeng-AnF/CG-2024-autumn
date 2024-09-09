#pragma once
#include "Graphics.h"
#include "Transform.h"
#include "InputState.h"
#include "InputStateMachine.h"

class Camera
{
public:
	Camera()noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void RotatePitchYaw(float dx, float dy)noexcept;
	void RotateRoll(float dz)noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	void IncreaseTravelSpeed();
	void DecreaseTravelSpeed();
	FTransform GetTransform()const;
	XMFLOAT3 GetForwardVector()const noexcept;
	XMFLOAT3 GetRightVector()const noexcept;
	XMFLOAT3 GetUpVector()const noexcept;
private:
	bool EnableRollRotate = false;
	FTransform transform;
	float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
};
//handel input
namespace InputStateClass {
	class CameraMove :public InputState
	{
	public:
		CameraMove(Window& wnd) :InputState(wnd) {};


		// Í¨¹ý InputState ¼Ì³Ð
		void Enter() override;
		void HandleKeyboardInput() override;
		void HandleMouseInput() override;
		void Update(float deltaTime) override;
		void Exit() override;
	};
}