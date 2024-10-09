#include "UBoxComponent.h"

UBoxComponent::UBoxComponent() {
    // 默认初始化盒子的边界
    Box = DirectX::BoundingBox();
}

UBoxComponent::~UBoxComponent() {}

void UBoxComponent::SetBoxExtent(const DirectX::XMFLOAT3& Extent) {
    Box.Extents = DirectX::XMFLOAT3(Extent.x, Extent.y, Extent.z);
    // 更新盒子的中心位置，假设中心在原点
    Box.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

const DirectX::BoundingBox& UBoxComponent::GetBoundingBox() const {
    return Box;
}

FHitResult UBoxComponent::CheckCollision(const UCollisionComponent& Other) const {
    FHitResult HitResult;

    // 这里根据不同的碰撞组件调用相应的碰撞检测方法
    if (const USphereComponent* Sphere = dynamic_cast<const USphereComponent*>(&Other)) {
        if (CheckCollisionWithSphere(*Sphere)) {
            // 填充 HitResult
        }
    }
    else if (const UCapsuleComponent* Capsule = dynamic_cast<const UCapsuleComponent*>(&Other)) {
        if (CheckCollisionWithCapsule(*Capsule)) {
            // 填充 HitResult
        }
    }
    else if (const UConvexComponent* Convex = dynamic_cast<const UConvexComponent*>(&Other)) {
        if (CheckCollisionWithConvex(*Convex)) {
            // 填充 HitResult
        }
    }

    return HitResult; // 返回碰撞结果
}

// 示例：与球体碰撞检测的实现
bool UBoxComponent::CheckCollisionWithSphere(const USphereComponent& Sphere) const {
    return Box.Intersects(Sphere.GetBoundingSphere());
}

// 示例：与胶囊体碰撞检测的实现
bool UBoxComponent::CheckCollisionWithCapsule(const UCapsuleComponent& Capsule) const {
    // 使用盒子与胶囊体的碰撞检测算法
    return Box.Intersects(Capsule.GetBoundingSphere());
}

// 示例：与凸多边形碰撞检测的实现
bool UBoxComponent::CheckCollisionWithConvex(const UConvexComponent& Convex) const {
    // 使用盒子与凸多边形的碰撞检测算法
    return Box.Intersects(Convex.GetConvexVertices());
}

