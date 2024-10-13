#pragma once
#include "UActorComponent.h"
#include "Transform.h"
#include <memory>
#include <string>
using namespace DirectX;

class UCharacterMovementComponent : public UActorComponent {
public:
    UCharacterMovementComponent(AActor*Owner);
    virtual ~UCharacterMovementComponent();

    // 每帧更新
    virtual void Update(float DeltaTime) override;

    // 设置移动输入
    void SetMovementInput(const XMVECTOR& Direction);

    // 跳跃
    void Jump();

    // 施加重力
    void ApplyGravity(float DeltaTime);

    // 获取当前速度
    XMVECTOR GetVelocity() const;

    XMVECTOR GetGravityDirection();
    void SetGravityDirection(XMVECTOR Direction);
public:
    const static std::string name;
//protected:
    XMVECTOR Velocity;         // 当前速度
    XMVECTOR Acceleration;     // 当前角色加速度，不含重力加速度
    XMVECTOR GravityDirection; // 重力方向

    float JumpInitialSpeed = 9.81f;                             // 跳跃初速度
    float GravityAcceleration = 9.81f;                          // 重力加速度
    float MaxSlopeAngle = 45.0f;                                // 最大爬坡坡度
    float maxAccAcceleration = 9.81f*2;                         // 最大地面加速加速度
    float maxDeceleration = 0.8*maxAccAcceleration;             // 最大地面减速加速度
    float maxGroundSpeed = 9.81f*1.5;                             // 最大地面速度
    float MaxFallingSpeed = 10 * GravityAcceleration;           // 最大下落速度
    bool bIsOnGround;                                           // 是否在地面上
    bool bIsInputAcceration;                                    // 是否有输入加速度
    float FloorDetectionThreshold = 0.7f;                       // 用于检测地板的法线阈值

    // 更新位置
    void UpdatePosition(float DeltaTime);
    void UpdateVelocity(float DeltaTime);
    void UpdateAcceleration(float DeltaTime);
private:
    bool IsWalkableSurface(const DirectX::XMVECTOR& Normal, float MaxSlopeAngle);

private:
    XMVECTOR InputAcceration;  // 当前角色移动平面的加速度
};