#pragma once
#include "APawn.h"
#include "UCharacterMovementComponent.h"


class ACharacter : public APawn {
public:
    // ��ɫ��״̬���������ߡ���Ծ�ȣ�
    enum class ECharacterState {
        Idle,
        Walking,
        Jumping
    };
    ACharacter();
    virtual ~ACharacter();

    // ÿ֡����
    virtual void Tick(float DeltaTime) override;

    // ���ý�ɫ�ƶ�������
    void Move(const DirectX::XMFLOAT3& Direction);

    // ���ý�ɫת�������
    void Turn(XMVECTOR deltaQuat);

    void AddYawInput(int x);
    void AddPitchInput(int x);
    void AddRollInput(int x);

    // x+ Ϊ��ǰ��y+Ϊ����
    void MoveInput(int x, int y);

    // ִ����Ծ
    virtual void Jump()override;

    // ������Ծ״̬
    virtual void UpdateJump(float DeltaTime)override;

    // �����ɫ״̬����
    void UpdateState(ECharacterState state);

    XMMATRIX GetCameraMatrix();
protected:
    // ��ɫ����ת�ٶ�
    float TurnSpeed = XM_PI/8; //rad per frame

public:
    ECharacterState CurrentState=ECharacterState::Idle;

    std::shared_ptr<UCharacterMovementComponent> CharacterMovementComponent;

    FTransform ControllerTransform;
private:
};