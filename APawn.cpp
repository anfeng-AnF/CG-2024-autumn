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

}

// ������Ծ״̬
void APawn::UpdateJump(float DeltaTime) {

}