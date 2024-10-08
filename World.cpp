#include "World.h"

// 添加 Actor
void UWorld::AddActor(std::shared_ptr<AActor> Actor) {
    ActiveGroupActors.insert(Actor);
}

// 移除 Actor
void UWorld::RemoveActor(std::shared_ptr<AActor> Actor) {
    ActiveGroupActors.erase(Actor);
}

// 每帧更新所有 Actor
void UWorld::Tick(float DeltaTime) {
    for (const auto& Actor : ActiveGroupActors) {
        if (Actor) {
            Actor->Tick(DeltaTime); // 假设 AActor 类有一个 Tick 函数
        }
    }
}

// 渲染所有 Actor
void UWorld::Render() {
    for (const auto& Actor : ActiveGroupActors) {
        if (Actor) {
            Actor->Render(); // 假设 AActor 类有一个 Render 函数
        }
    }
}
