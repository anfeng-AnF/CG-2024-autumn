#include "APawn.h"

void APawn::Tick(float DeltaTime) {
    AActor::Tick(DeltaTime); // 调用基类的 Tick 函数
}

// 设置移动方向
void APawn::SetMovementDirection(const DirectX::XMFLOAT3& NewDirection) {
    MovementDirection = NewDirection;
}

// 执行跳跃
void APawn::Jump() {

}

// 更新跳跃状态
void APawn::UpdateJump(float DeltaTime) {

}