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
    XMVECTOR Velocity;         // �ٶȷ���
    XMVECTOR Speed;            // �ٶȴ�С
    XMVECTOR GravityDirection; // ��������
    XMVECTOR MovementInput;    // �ƶ�����
    float JumpHeight;          // ��Ծ�߶�
    bool bIsOnGround;          // �Ƿ��ڵ�����
    float GravityScale;        // �������ٶ�
    float MaxSlopeAngle = 45.0f;// ��������¶�
    float FloorDetectionThreshold = 0.7f; // ���ڼ��ذ�ķ�����ֵ
    // ����λ��
    void UpdatePosition(float DeltaTime);
    void UpdateVelocity(float DeltaTime);

private:
    bool IsWalkableSurface(const DirectX::XMVECTOR& Normal, float MaxSlopeAngle);
};