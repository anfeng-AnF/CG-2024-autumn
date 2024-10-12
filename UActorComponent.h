#pragma once
#include <DirectXCollision.h>
#include <memory>
#include <string>
#include "Graphics.h"
#include "Transform.h"
class AActor;
class UActorComponent {
public:
    UActorComponent() {}
    virtual ~UActorComponent() {}

    // 初始化组件
    virtual void Initialize() {}

    // 更新组件
    virtual void Update(float DeltaTime) = 0;

    // 渲染组件
    virtual void Render(Graphics& gfx) {}

    // 设置所属的 Actor
    void SetOwner(AActor* NewOwner);

    virtual DirectX::XMMATRIX GetRelationTransformMatrix();

    // 获取所属的 Actor
    AActor* GetOwner() const;

    FTransform RelationTransform;
protected:
    AActor* Owner; // 指向所属的 Actor
};
