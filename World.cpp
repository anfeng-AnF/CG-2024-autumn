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

        }
    }
}

// 渲染所有 Actor
void UWorld::Render() {
    for (const auto& Actor : ActiveGroupActors) {
        if (Actor) {

        }
    }
}

std::vector<FCollisionResult> UWorld::QuaryCollision(std::shared_ptr<AActor> Actor)
{
    return {};
}
