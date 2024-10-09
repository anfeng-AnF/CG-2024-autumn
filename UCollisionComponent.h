#pragma once
#include "UActorComponent.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <memory>
struct FHitResult {
    bool bHit;                  // 是否发生碰撞
    std::shared_ptr<AActor> Actor; // 碰撞的 Actor
    DirectX::XMFLOAT3 Location; // 碰撞位置
    DirectX::XMFLOAT3 Normal;   // 碰撞法线

    FHitResult() : bHit(false), Actor(nullptr), Location({ 0, 0, 0 }), Normal({ 0, 0, 0 }){}
};

// 碰撞组件基类
class UCollisionComponent : public UActorComponent {
public:
    UCollisionComponent() {}
    virtual ~UCollisionComponent() {}

    // 纯虚函数，检测与其他碰撞组件的碰撞
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const = 0;

protected:
    
};