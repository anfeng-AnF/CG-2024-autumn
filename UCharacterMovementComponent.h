#pragma once
#include "UActorComponent.h"
#include "Transform.h"
#include <memory>
using namespace DirectX;

class UCharacterMovementComponent : public UActorComponent {
public:
    UCharacterMovementComponent();
    virtual ~UCharacterMovementComponent();

    // ÿ֡����
    virtual void Update(float DeltaTime) override;

    // �����ƶ�����
    void SetMovementInput(const XMFLOAT3& Direction);

    // ��Ծ
    void Jump();

    // ʩ������
    void ApplyGravity(float DeltaTime);

    // ��ȡ��ǰ�ٶ�
    XMVECTOR GetVelocity() const;
public:
    const static std::string name;
protected:
    // ��ǰ�ٶ�
    XMVECTOR Velocity;

    // �ƶ�����
    XMFLOAT3 MovementInput;

    // ��Ծ�߶�
    float JumpHeight;

    // �Ƿ��ڵ�����
    bool bIsOnGround;

    // �������ٶ�
    float GravityScale;

    // ����λ��
    void UpdatePosition(float DeltaTime);
};