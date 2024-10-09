#pragma once
#include "UCollisionComponent.h"

class UCapsuleComponent : public UCollisionComponent {
public:
    UCapsuleComponent();
    virtual ~UCapsuleComponent();

    // ���ý�����İ뾶�͸߶�
    void SetCapsule(float Radius, float Height);

    // ��ȡ������İ뾶�͸߶�
    float GetRadius() const;
    float GetHeight() const;

    // ʵ�ּ����ײ�ĺ���
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;

private:
    float Radius; // ������İ뾶
    float Height; // ������ĸ߶�
    DirectX::XMFLOAT3 Center; // �����������λ��

    // ���㽺����ı߽�
    DirectX::BoundingBox GetBoundingBox() const;
    DirectX::BoundingSphere GetBoundingSphere() const;
};

