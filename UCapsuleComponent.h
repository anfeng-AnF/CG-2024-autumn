#pragma once
#include "UCollisionComponent.h"

class UCapsuleComponent : public UCollisionComponent {
public:
    UCapsuleComponent();
    virtual ~UCapsuleComponent();

    // 设置胶囊体的半径和高度
    void SetCapsule(float Radius, float Height);

    // 获取胶囊体的半径和高度
    float GetRadius() const;
    float GetHeight() const;

    // 实现检查碰撞的函数
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;

private:
    float Radius; // 胶囊体的半径
    float Height; // 胶囊体的高度
    DirectX::XMFLOAT3 Center; // 胶囊体的中心位置

    // 计算胶囊体的边界
    DirectX::BoundingBox GetBoundingBox() const;
    DirectX::BoundingSphere GetBoundingSphere() const;
};

