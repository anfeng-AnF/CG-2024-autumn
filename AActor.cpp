#include "AActor.h"

// 获取变换
const FTransform& AActor::GetTransform() const {
    return Transform;
}

// 设置变换
void AActor::SetTransform(const FTransform& NewTransform) {
    Transform = NewTransform;
}

// 每帧更新
void AActor::Tick(float DeltaTime) {
    // 更新逻辑，例如移动、旋转等
}

// 渲染函数
void AActor::Render() {
    // 渲染逻辑
}

// 处理碰撞
void AActor::OnCollision() {
    // 碰撞处理逻辑
}

// 添加组件
void AActor::AddComponent(std::unique_ptr<UActorComponent> NewComponent) {
    if (!RootComponent) {
        RootComponent = std::move(NewComponent);
    }
    // 你可以扩展这个逻辑以支持多个组件
}
