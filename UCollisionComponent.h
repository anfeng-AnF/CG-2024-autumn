#pragma once
#include "UActorComponent.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <memory>
struct FHitResult {
    bool bHit;                  // �Ƿ�����ײ
    std::shared_ptr<AActor> Actor; // ��ײ�� Actor
    DirectX::XMFLOAT3 Location; // ��ײλ��
    DirectX::XMFLOAT3 Normal;   // ��ײ����

    FHitResult() : bHit(false), Actor(nullptr), Location({ 0, 0, 0 }), Normal({ 0, 0, 0 }){}
};

// ��ײ�������
class UCollisionComponent : public UActorComponent {
public:
    UCollisionComponent() {}
    virtual ~UCollisionComponent() {}

    // ���麯���������������ײ�������ײ
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const = 0;

protected:
    
};