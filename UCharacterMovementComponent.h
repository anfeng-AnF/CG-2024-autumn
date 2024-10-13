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

    // ÿ֡����
    virtual void Update(float DeltaTime) override;

    // �����ƶ�����
    void SetMovementInput(const XMVECTOR& Direction);

    // ��Ծ
    void Jump();

    // ʩ������
    void ApplyGravity(float DeltaTime);

    // ��ȡ��ǰ�ٶ�
    XMVECTOR GetVelocity() const;

    XMVECTOR GetGravityDirection();
    void SetGravityDirection(XMVECTOR Direction);
public:
    const static std::string name;
//protected:
    XMVECTOR Velocity;         // ��ǰ�ٶ�
    XMVECTOR Acceleration;     // ��ǰ��ɫ���ٶȣ������������ٶ�
    XMVECTOR GravityDirection; // ��������

    float JumpInitialSpeed = 9.81f;                             // ��Ծ���ٶ�
    float GravityAcceleration = 9.81f;                          // �������ٶ�
    float MaxSlopeAngle = 45.0f;                                // ��������¶�
    float maxAccAcceleration = 9.81f*2;                         // ��������ټ��ٶ�
    float maxDeceleration = 0.8*maxAccAcceleration;             // ��������ټ��ٶ�
    float maxGroundSpeed = 9.81f*1.5;                             // �������ٶ�
    float MaxFallingSpeed = 10 * GravityAcceleration;           // ��������ٶ�
    bool bIsOnGround;                                           // �Ƿ��ڵ�����
    bool bIsInputAcceration;                                    // �Ƿ���������ٶ�
    float FloorDetectionThreshold = 0.7f;                       // ���ڼ��ذ�ķ�����ֵ

    // ����λ��
    void UpdatePosition(float DeltaTime);
    void UpdateVelocity(float DeltaTime);
    void UpdateAcceleration(float DeltaTime);
private:
    bool IsWalkableSurface(const DirectX::XMVECTOR& Normal, float MaxSlopeAngle);

private:
    XMVECTOR InputAcceration;  // ��ǰ��ɫ�ƶ�ƽ��ļ��ٶ�
};