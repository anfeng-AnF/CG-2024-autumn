#pragma once
#include "UActorComponent.h"
#include <DirectXMath.h>
#include <memory>

class AActor;

struct FHitResult {
    bool bHit;                  // 是否发生碰撞
    std::shared_ptr<UActorComponent> ActorComponent; // 碰撞的 ActorComponent
    DirectX::XMFLOAT3 Location; // 碰撞位置
    DirectX::XMFLOAT3 Normal;   // 碰撞法线

    FHitResult() : bHit(false), ActorComponent(nullptr), Location({ 0, 0, 0 }), Normal({ 0, 0, 0 }){}
};

// 碰撞组件基类
class UCollisionComponent : public UActorComponent {
public:
    UCollisionComponent() {}
    virtual ~UCollisionComponent() {}

    // 纯虚函数，检测与其他碰撞组件的碰撞
    virtual FHitResult CheckCollision(const UCollisionComponent& Other) const = 0;
    virtual FHitResult TraceByLine(XMFLOAT3 LineOrigin, XMFLOAT3 LineDirection) const = 0;
protected:
    
};