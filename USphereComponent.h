#pragma once
#include "UCollisionComponent.h"

class USphereComponent : public UCollisionComponent {
public:
    USphereComponent();
    virtual ~USphereComponent();

    // ��������İ뾶
    void SetSphereRadius(float Radius);

    // ��ȡ����ļ�����
    const DirectX::BoundingSphere& GetBoundingSphere() const;

    // ʵ�ּ����ײ�ĺ���
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;

    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const override;
private:
    DirectX::BoundingSphere Sphere; // �������ײ������
};
