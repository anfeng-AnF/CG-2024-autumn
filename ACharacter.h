#pragma once
#include "APawn.h"
#include "UCharacterMovementComponent.h"


class ACharacter : public APawn {
public:
    ACharacter();
    virtual ~ACharacter();

    // 每帧更新
    virtual void Tick(float DeltaTime) override;

    // 设置角色移动的输入
    void Move(const DirectX::XMFLOAT3& Direction);

    // 设置角色转向的输入
    void Turn(float Angle);

    // x+ 为向前，y+为向右
    void Input(int x, int y);

protected:
    // 角色的旋转速度
    float TurnSpeed;

    GameInput* PlayerController;

    // 角色的状态（例如行走、跳跃等）
    enum class ECharacterState {
        Idle,
        Walking,
        Jumping
    };

public:
    ECharacterState CurrentState;

    UCharacterMovementComponent* CharacterMovementComponent;


private:
    // 处理角色状态更新
    void UpdateState();
};