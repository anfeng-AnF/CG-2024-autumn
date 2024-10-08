#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Transform.h"
#include "UActorComponent.h"
#include "vector"

struct Component
{
    std::unique_ptr<UActorComponent> ActorComponent;
    std::vector<Component> ChildComponent;
};

class AActor {
public:
    AActor() {};
    virtual ~AActor() {};

    // 获取变换
    const FTransform& GetTransform() const;

    // 设置变换
    void SetTransform(const FTransform& NewTransform);

    // 每帧更新
    virtual void Tick(float DeltaTime);

    // 渲染函数（可以覆盖）
    virtual void Render();

    // 处理碰撞（如果需要）
    virtual void OnCollision();

    // 添加组件
    void AddComponent(std::unique_ptr<UActorComponent> NewComponent);
protected:

    FTransform Transform; // 变换
    std::unique_ptr<Component> RootComponent;
};