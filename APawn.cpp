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
    if (!bIsJumping) {
        bIsJumping = true;
        Velocity.y = JumpForce; // 设置向上的速度
    }
}

// 更新跳跃状态
void APawn::UpdateJump(float DeltaTime) {
    if (bIsJumping) {
        // 应用重力
        Velocity.y += Gravity * DeltaTime;

        // 如果在地面上，停止跳跃
        if (Transform.position.y <= 0.0f) { // 假设地面的 Y 坐标为 0
            Transform.position.y = 0.0f; // 确保位置不低于地面
            bIsJumping = false; // 结束跳跃状态
            Velocity.y = 0.0f; // 重置速度
        }
    }
}