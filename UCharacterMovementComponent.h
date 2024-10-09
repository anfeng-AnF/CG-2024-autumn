#pragma once
#include "UActorComponent.h"
#include "Transform.h"
#include <memory>
using namespace DirectX;

class UCharacterMovementComponent : public UActorComponent {
public:
    UCharacterMovementComponent();
    virtual ~UCharacterMovementComponent();

    // 每帧更新
    virtual void Update(float DeltaTime) override;

    // 设置移动输入
    void SetMovementInput(const XMFLOAT3& Direction);

    // 跳跃
    void Jump();

    // 施加重力
    void ApplyGravity(float DeltaTime);

    // 获取当前速度
    XMVECTOR GetVelocity() const;
public:
    const static std::string name;
protected:
    // 当前速度
    XMVECTOR Velocity;

    // 移动方向
    XMFLOAT3 MovementInput;

    // 跳跃高度
    float JumpHeight;

    // 是否在地面上
    bool bIsOnGround;

    // 重力加速度
    float GravityScale;

    // 更新位置
    void UpdatePosition(float DeltaTime);
};