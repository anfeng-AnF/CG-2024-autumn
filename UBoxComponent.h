#pragma once
#include "UCollisionComponent.h"

class USphereComponent;
class UCapsuleComponent;
class UConvexComponent;

class UBoxComponent : public UCollisionComponent {
public:
    UBoxComponent();
    virtual ~UBoxComponent();

    // 设置盒子的尺寸
    void SetBoxExtent(const DirectX::XMFLOAT3& Extent);

    // 获取盒子的几何体
    const DirectX::BoundingBox& GetBoundingBox() const;

    // 实现检查碰撞的函数
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;
    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const override;
private:
    // 盒子的碰撞几何体
    DirectX::BoundingBox Box;

    // 与其他碰撞体的碰撞检测
    bool CheckCollisionWithSphere(const USphereComponent& Sphere) const;
    bool CheckCollisionWithCapsule(const UCapsuleComponent& Capsule) const;
    bool CheckCollisionWithConvex(const UConvexComponent& Convex) const;
};
