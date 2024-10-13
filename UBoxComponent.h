#pragma once
#include "UCollisionComponent.h"

class USphereComponent;
class UCapsuleComponent;
class UConvexComponent;

class UBoxComponent : public UCollisionComponent {
public:
    UBoxComponent();
    virtual ~UBoxComponent();

    // ���ú��ӵĳߴ�
    void SetBoxExtent(const DirectX::XMFLOAT3& Extent);

    // ��ȡ���ӵļ�����
    const DirectX::BoundingBox& GetBoundingBox() const;

    // ʵ�ּ����ײ�ĺ���
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;
    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const override;
private:
    // ���ӵ���ײ������
    DirectX::BoundingBox Box;

    // ��������ײ�����ײ���
    bool CheckCollisionWithSphere(const USphereComponent& Sphere) const;
    bool CheckCollisionWithCapsule(const UCapsuleComponent& Capsule) const;
    bool CheckCollisionWithConvex(const UConvexComponent& Convex) const;
};
