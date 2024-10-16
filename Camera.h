#pragma once
#include "Graphics.h"
#include "Transform.h"
#include "InputStateMachine.h"
#include <memory>
class Camera
{
public:
	class CameraMove :public InputState
	{
	public:
		CameraMove(Window& window, Camera* cam) : InputState(window), cam(cam) {};
		// ͨ�� InputState �̳�
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;

	private:
		Camera* cam;

		// ͨ�� InputState �̳�
		void Draw() override;
	};
	std::unique_ptr<CameraMove> inputState;
public:
	Camera() = default;
	Camera(Window& window);
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void SetTransform(FTransform transform);
	void RotatePitchYaw(float dx, float dy)noexcept;
	void RotateRoll(float dz)noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	void IncreaseTravelSpeed();
	void DecreaseTravelSpeed();
	FTransform GetTransform()const;
	XMFLOAT3 GetForwardVector()const noexcept;
	XMFLOAT3 GetRightVector()const noexcept;
	XMFLOAT3 GetUpVector()const noexcept;

public:
	static const FTransform FrontView;
	static const FTransform RightView;
	static const FTransform TopView  ;

private:
	bool EnableRollRotate = false;
	FTransform transform;
	float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
};