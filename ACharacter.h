#pragma once
#include "APawn.h"
#include "UCharacterMovementComponent.h"


class ACharacter : public APawn {
public:
    ACharacter();
    virtual ~ACharacter();

    // ÿ֡����
    virtual void Tick(float DeltaTime) override;

    // ���ý�ɫ�ƶ�������
    void Move(const DirectX::XMFLOAT3& Direction);

    // ���ý�ɫת�������
    void Turn(float Angle);

    // x+ Ϊ��ǰ��y+Ϊ����
    void Input(int x, int y);

protected:
    // ��ɫ����ת�ٶ�
    float TurnSpeed;

    GameInput* PlayerController;

    // ��ɫ��״̬���������ߡ���Ծ�ȣ�
    enum class ECharacterState {
        Idle,
        Walking,
        Jumping
    };

public:
    ECharacterState CurrentState;

    UCharacterMovementComponent* CharacterMovementComponent;


private:
    // �����ɫ״̬����
    void UpdateState();
};