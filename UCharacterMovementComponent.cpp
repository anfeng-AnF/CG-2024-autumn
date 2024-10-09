#include "UCharacterMovementComponent.h"

const std::string UCharacterMovementComponent::name = "UCharacterMovementComponent";

// ���캯��
UCharacterMovementComponent::UCharacterMovementComponent()
    : Velocity(XMVectorZero()), // ��ʼ���ٶ�Ϊ������
    MovementInput(XMFLOAT3(0.0f, 0.0f, 0.0f)), // ��ʼ���ƶ�����Ϊ��
    JumpHeight(300.0f), // ������Ծ�߶�
    bIsOnGround(true), // Ĭ���ڵ�����
    GravityScale(9.81f) // �����������ٶ�
{}

// ��������
UCharacterMovementComponent::~UCharacterMovementComponent() {}

// ÿ֡����
void UCharacterMovementComponent::Update(float DeltaTime) {
    // ��Ӧ������
    ApplyGravity(DeltaTime);

    // ����λ��
    UpdatePosition(DeltaTime);
}

// �����ƶ�����
void UCharacterMovementComponent::SetMovementInput(const XMFLOAT3& Direction) {
    MovementInput = Direction;
}

// ��Ծ
void UCharacterMovementComponent::Jump() {

}

// ʩ������
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {

}

// ��ȡ��ǰ�ٶ�
XMVECTOR UCharacterMovementComponent::GetVelocity() const {
    return Velocity;
}

// ����λ��
void UCharacterMovementComponent::UpdatePosition(float DeltaTime) {

}