#pragma once
#include "UActorComponent.h"
#include <DirectXMath.h>
#include <memory>

class AActor;

struct FHitResult {
    bool bHit;                  // �Ƿ�����ײ
    std::shared_ptr<UActorComponent> ActorComponent; // ��ײ�� ActorComponent
    DirectX::XMFLOAT3 Location; // ��ײλ��
    DirectX::XMFLOAT3 Normal;   // ��ײ����

    FHitResult() : bHit(false), ActorComponent(nullptr), Location({ 0, 0, 0 }), Normal({ 0, 0, 0 }){}
};

// ��ײ�������
class UCollisionComponent : public UActorComponent {
public:
    UCollisionComponent() {}
    virtual ~UCollisionComponent() {}

    // ���麯���������������ײ�������ײ
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const = 0;
    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const = 0;
protected:
    
};