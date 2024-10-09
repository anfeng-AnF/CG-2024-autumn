#pragma once
#include "UCollisionComponent.h"
#include <vector>

class UConvexComponent : public UCollisionComponent {
public:
    UConvexComponent();
    virtual ~UConvexComponent();

    // 设置凸多边形的顶点
    void SetConvexVertices(const std::vector<DirectX::XMFLOAT3>& Vertices);

    // 获取凸多边形的几何体
    const std::vector<DirectX::XMFLOAT3>& GetConvexVertices() const;

    // 实现检查碰撞的函数
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const override;

private:
    std::vector<DirectX::XMFLOAT3> ConvexVertices;
    std::vector<int> indeices;
};
