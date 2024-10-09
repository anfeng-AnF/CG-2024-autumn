#include "APawn.h"

void APawn::Tick(float DeltaTime) {
    AActor::Tick(DeltaTime); // ���û���� Tick ����
}

// �����ƶ�����
void APawn::SetMovementDirection(const DirectX::XMFLOAT3& NewDirection) {
    MovementDirection = NewDirection;
}

// ִ����Ծ
void APawn::Jump() {
    if (!bIsJumping) {
        bIsJumping = true;
        Velocity.y = JumpForce; // �������ϵ��ٶ�
    }
}

// ������Ծ״̬
void APawn::UpdateJump(float DeltaTime) {
    if (bIsJumping) {
        // Ӧ������
        Velocity.y += Gravity * DeltaTime;

        // ����ڵ����ϣ�ֹͣ��Ծ
        if (Transform.position.y <= 0.0f) { // �������� Y ����Ϊ 0
            Transform.position.y = 0.0f; // ȷ��λ�ò����ڵ���
            bIsJumping = false; // ������Ծ״̬
            Velocity.y = 0.0f; // �����ٶ�
        }
    }
}