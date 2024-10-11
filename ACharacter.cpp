#include "ACharacter.h"
#include <DirectXMathVector.inl>
ACharacter::ACharacter()
{
}

ACharacter::~ACharacter()
{
}

// 每帧更新
void ACharacter::Tick(float DeltaTime) {
    APawn::Tick(DeltaTime); // 调用基类的 Tick 函数

    // 更新角色状态
    UpdateState();
}

// 设置角色移动的输入
void ACharacter::Move(const DirectX::XMFLOAT3& Direction) {
    // 这里可以根据方向和速度更新移动方向
    MovementDirection = Direction; // 更新移动方向
}

// 设置角色转向的输入
void ACharacter::Turn(float Angle) {
    // 角色转向逻辑，简单的将旋转角度添加到当前旋转
    DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, Angle * TurnSpeed, 0.0f);
    Transform.rotation = DirectX::XMQuaternionMultiply(Transform.rotation, rotationQuaternion); // 更新角色旋转
}

void ACharacter::Input(int x, int y)
{
    auto forward = Transform.GetForwardVector()* x;
    auto right = Transform.GetRightVector()* y;
}

// 更新角色状态
void ACharacter::UpdateState() {
    // 根据当前移动和跳跃状态更新角色的状态
    if (bIsJumping) {
        CurrentState = ECharacterState::Jumping;
    }
    else if (MovementDirection.x != 0.0f || MovementDirection.z != 0.0f) {
        CurrentState = ECharacterState::Walking;
    }
    else {
        CurrentState = ECharacterState::Idle;
    }

    // 这里可以扩展更多的状态逻辑，例如播放不同的动画等
}

