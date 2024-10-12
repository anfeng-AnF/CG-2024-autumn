#include "ACharacter.h"
#include "USpringArmComponent.h"
#include "UCameraComponent.h"
#include "UCapsuleComponent.h"
#include <DirectXMathVector.inl>
ACharacter::ACharacter()
    :
    CharacterMovementComponent(std::make_shared<UCharacterMovementComponent>()),
    APawn()
{
    //默认添加弹簧臂组建和摄像机组件
    AddComponent("", std::make_shared<UCapsuleComponent>(), UCapsuleComponent::name);                       //root
    AddComponent(AActor::RootComponentName, std::make_shared<USpringArmComponent>(), USpringArmComponent::name);
    AddComponent(USpringArmComponent::name, std::make_shared<UCameraComponent>(std::make_shared<Camera>()), UCameraComponent::name);
}

ACharacter::~ACharacter()
{
}

// 每帧更新
void ACharacter::Tick(float DeltaTime) {
    APawn::Tick(DeltaTime); // 调用基类的 Tick 函数

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

void ACharacter::AddYawInput(int x)
{
    XMVECTOR deltaRotation = XMQuaternionRotationNormal(Transform.GetUpVector(), (float)x);
    Transform.rotation = DirectX::XMQuaternionMultiply(Transform.rotation, deltaRotation);
}

void ACharacter::AddPitchInput(int x)
{
    XMVECTOR deltaRotation = XMQuaternionRotationNormal(Transform.GetRightVector(), (float)x);
    Transform.rotation = DirectX::XMQuaternionMultiply(Transform.rotation, deltaRotation);
}

void ACharacter::AddRollInput(int x)
{
    XMVECTOR deltaRotation = XMQuaternionRotationNormal(Transform.GetForwardVector(), (float)x);
    Transform.rotation = DirectX::XMQuaternionMultiply(Transform.rotation, deltaRotation);
}

void ACharacter::MoveInput(int x, int y)
{
    float length = x * x + y * y;
    float dx = x * x / length;
    float dy = y * y / length;
    XMVECTOR forward = Transform.GetForwardVector()* dx;
    XMVECTOR right = Transform.GetRightVector()* dy;
    CharacterMovementComponent->SetMovementInput(forward + right);
}

void ACharacter::Jump()
{
    CharacterMovementComponent->Jump();
}

void ACharacter::UpdateJump(float DeltaTime)
{
}

// 更新角色状态
void ACharacter::UpdateState(ECharacterState state) {
    CurrentState = state;
}

XMMATRIX ACharacter::GetCameraMatrix()
{
    if (Components.find(UCameraComponent::name) != Components.end()) {
        if (auto camComp = dynamic_cast<UCameraComponent*>(Components[UCameraComponent::name]->ActorComponent.get())) {
            return camComp->GetMatrix();
        }
    }
    return XMMatrixIdentity();
}

