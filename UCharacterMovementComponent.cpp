#include "UCharacterMovementComponent.h"

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
    if (bIsOnGround) {
        // ʩ����Ծ��
        Velocity.y = JumpHeight; // ����򵥵ؽ�Y���ٶ�����Ϊ��Ծ�߶�
        bIsOnGround = false; // ��Ծ�����ڵ�����
    }
}

// ʩ������
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {
    if (!bIsOnGround) {
        // ÿ֡ʩ������
        Velocity.y -= GravityScale * DeltaTime; // ����Y���ٶ�
    }
}

// ��ȡ��ǰ�ٶ�
XMVECTOR UCharacterMovementComponent::GetVelocity() const {
    return Velocity;
}

// ����λ��
void UCharacterMovementComponent::UpdatePosition(float DeltaTime) {
    // �����µ�λ��
    XMVECTOR Movement = XMVectorSet(MovementInput.x, 0.0f, MovementInput.z, 0.0f);
    Velocity += Movement; // �����ٶ�

    // ����λ�ã����Ը�����Ҫ������ײ��⣩
    XMVECTOR NewPosition = GetOwner()->GetTransform().GetMatrix().r[3] + (Velocity * DeltaTime);
    GetOwner()->SetTransform(FTransform(NewPosition)); // ���±任
}