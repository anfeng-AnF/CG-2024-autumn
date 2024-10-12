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
    //Ĭ����ӵ��ɱ��齨����������
    AddComponent("", std::make_shared<UCapsuleComponent>(), UCapsuleComponent::name);                       //root
    AddComponent(AActor::RootComponentName, std::make_shared<USpringArmComponent>(), USpringArmComponent::name);
    AddComponent(USpringArmComponent::name, std::make_shared<UCameraComponent>(std::make_shared<Camera>()), UCameraComponent::name);
}

ACharacter::~ACharacter()
{
}

// ÿ֡����
void ACharacter::Tick(float DeltaTime) {
    APawn::Tick(DeltaTime); // ���û���� Tick ����

}

// ���ý�ɫ�ƶ�������
void ACharacter::Move(const DirectX::XMFLOAT3& Direction) {
    // ������Ը��ݷ�����ٶȸ����ƶ�����
    MovementDirection = Direction; // �����ƶ�����
}

// ���ý�ɫת�������
void ACharacter::Turn(float Angle) {
    // ��ɫת���߼����򵥵Ľ���ת�Ƕ���ӵ���ǰ��ת
    DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, Angle * TurnSpeed, 0.0f);
    Transform.rotation = DirectX::XMQuaternionMultiply(Transform.rotation, rotationQuaternion); // ���½�ɫ��ת
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

// ���½�ɫ״̬
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

