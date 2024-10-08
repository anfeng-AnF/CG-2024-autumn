#include "UCharacterMovementComponent.h"

// 构造函数
UCharacterMovementComponent::UCharacterMovementComponent()
    : Velocity(XMVectorZero()), // 初始化速度为零向量
    MovementInput(XMFLOAT3(0.0f, 0.0f, 0.0f)), // 初始化移动输入为零
    JumpHeight(300.0f), // 设置跳跃高度
    bIsOnGround(true), // 默认在地面上
    GravityScale(9.81f) // 设置重力加速度
{}

// 析构函数
UCharacterMovementComponent::~UCharacterMovementComponent() {}

// 每帧更新
void UCharacterMovementComponent::Update(float DeltaTime) {
    // 先应用重力
    ApplyGravity(DeltaTime);

    // 更新位置
    UpdatePosition(DeltaTime);
}

// 设置移动输入
void UCharacterMovementComponent::SetMovementInput(const XMFLOAT3& Direction) {
    MovementInput = Direction;
}

// 跳跃
void UCharacterMovementComponent::Jump() {
    if (bIsOnGround) {
        // 施加跳跃力
        Velocity.y = JumpHeight; // 这里简单地将Y轴速度设置为跳跃高度
        bIsOnGround = false; // 跳跃后不再在地面上
    }
}

// 施加重力
void UCharacterMovementComponent::ApplyGravity(float DeltaTime) {
    if (!bIsOnGround) {
        // 每帧施加重力
        Velocity.y -= GravityScale * DeltaTime; // 更新Y轴速度
    }
}

// 获取当前速度
XMVECTOR UCharacterMovementComponent::GetVelocity() const {
    return Velocity;
}

// 更新位置
void UCharacterMovementComponent::UpdatePosition(float DeltaTime) {
    // 计算新的位置
    XMVECTOR Movement = XMVectorSet(MovementInput.x, 0.0f, MovementInput.z, 0.0f);
    Velocity += Movement; // 更新速度

    // 更新位置（可以根据需要进行碰撞检测）
    XMVECTOR NewPosition = GetOwner()->GetTransform().GetMatrix().r[3] + (Velocity * DeltaTime);
    GetOwner()->SetTransform(FTransform(NewPosition)); // 更新变换
}