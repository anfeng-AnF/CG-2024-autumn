#pragma once
#include <DirectXCollision.h>
#include <memory>
#include "AActor.h"

class UActorComponent {
public:
    UActorComponent() {}
    virtual ~UActorComponent() {}

    // 初始化组件
    virtual void Initialize() {}

    // 更新组件
    virtual void Update(float DeltaTime) = 0;

    // 渲染组件
    virtual void Render() {}

    // 设置所属的 Actor
    void SetOwner(AActor* NewOwner);

    // 获取所属的 Actor
    std::shared_ptr<AActor> GetOwner() const;

protected:
    std::shared_ptr<AActor> Owner; // 指向所属的 Actor
};
