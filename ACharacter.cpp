#include "ACharacter.h"
#include <DirectXMathVector.inl>
ACharacter::ACharacter()
{
}

ACharacter::~ACharacter()
{
}

// ÿ֡����
void ACharacter::Tick(float DeltaTime) {
    APawn::Tick(DeltaTime); // ���û���� Tick ����

    // ���½�ɫ״̬
    UpdateState();
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

void ACharacter::Input(int x, int y)
{
    auto forward = Transform.GetForwardVector()* x;
    auto right = Transform.GetRightVector()* y;
}

// ���½�ɫ״̬
void ACharacter::UpdateState() {
    // ���ݵ�ǰ�ƶ�����Ծ״̬���½�ɫ��״̬
    if (bIsJumping) {
        CurrentState = ECharacterState::Jumping;
    }
    else if (MovementDirection.x != 0.0f || MovementDirection.z != 0.0f) {
        CurrentState = ECharacterState::Walking;
    }
    else {
        CurrentState = ECharacterState::Idle;
    }

    // ���������չ�����״̬�߼������粥�Ų�ͬ�Ķ�����
}

