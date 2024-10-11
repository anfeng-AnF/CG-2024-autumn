#include <memory>
#include <DirectXMath.h>
#include "AActor.h"

class APawn : public AActor {
public:
    APawn();
    virtual ~APawn();

    // 每帧更新
    virtual void Tick(float DeltaTime) override;

    // 设置移动方向
    void SetMovementDirection(const DirectX::XMFLOAT3& NewDirection);

    // 执行跳跃
    virtual void Jump();

    // 更新跳跃状态
    virtual void UpdateJump(float DeltaTime);

protected:
    // 移动速度
    float MoveSpeed;

    // 跳跃力量
    float JumpForce;

    // 当前移动方向
    DirectX::XMFLOAT3 MovementDirection;

    // 跳跃状态
    bool bIsJumping;

    // 重力加速度
    float Gravity;

    // 当前速度
    DirectX::XMFLOAT3 Velocity;
};
