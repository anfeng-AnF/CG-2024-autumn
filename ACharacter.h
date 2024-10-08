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

protected:
    // ��ɫ����ת�ٶ�
    float TurnSpeed;

    // ��ɫ��״̬���������ߡ���Ծ�ȣ�
    enum class ECharacterState {
        Idle,
        Walking,
        Jumping
    };

    ECharacterState CurrentState;

    UCharacterMovementComponent* CharacterMovementComponent;

    // �����ɫ״̬����
    void UpdateState();
};