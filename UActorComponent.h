#pragma once

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
    virtual void Render() {}

    // 设置所属的 Actor
    void SetOwner(AActor* NewOwner);

    // 获取所属的 Actor
    AActor* GetOwner() const;

protected:
    AActor* Owner; // 指向所属的 Actor
};
