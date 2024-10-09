#pragma once
#include "UActorComponent.h"
#include "Transform.h"
#include <memory>
#include <string>
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
    XMVECTOR Velocity;         // 当前速度
    XMFLOAT3 MovementInput;    // 移动方向
    float JumpHeight;          // 跳跃高度
    bool bIsOnGround;          // 是否在地面上
    float GravityScale;        // 重力加速度

    // 更新位置
    void UpdatePosition(float DeltaTime);
};