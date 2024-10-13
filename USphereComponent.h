#pragma once
#include "UCollisionComponent.h"

class USphereComponent : public UCollisionComponent {
public:
    USphereComponent();
    virtual ~USphereComponent();

    // 设置球体的半径
    void SetSphereRadius(float Radius);

    // 获取球体的几何体
    const DirectX::BoundingSphere& GetBoundingSphere() const;

    // 实现检查碰撞的函数
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;

    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const override;
private:
    DirectX::BoundingSphere Sphere; // 球体的碰撞几何体
};
