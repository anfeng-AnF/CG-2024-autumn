#pragma once
#include "APawn.h"
#include "UCharacterMovementComponent.h"


class ACharacter : public APawn {
public:
    // 角色的状态（例如行走、跳跃等）
    enum class ECharacterState {
        Idle,
        Walking,
        Jumping
    };
    ACharacter();
    virtual ~ACharacter();

    // 每帧更新
    virtual void Tick(float DeltaTime) override;

    // 设置角色移动的输入
    void Move(const DirectX::XMFLOAT3& Direction);

    // 设置角色转向的输入
    void Turn(float Angle);

    void AddYawInput(int x);
    void AddPitchInput(int x);
    void AddRollInput(int x);

    // x+ 为向前，y+为向右
    void MoveInput(int x, int y);

    // 执行跳跃
    virtual void Jump()override;

    // 更新跳跃状态
    virtual void UpdateJump(float DeltaTime)override;

    // 处理角色状态更新
    void UpdateState(ECharacterState state);

    XMMATRIX GetCameraMatrix();
protected:
    // 角色的旋转速度
    float TurnSpeed = 90.f;

public:
    ECharacterState CurrentState=ECharacterState::Idle;

    std::shared_ptr<UCharacterMovementComponent> CharacterMovementComponent;

    FTransform ControllerTransform;
private:
};