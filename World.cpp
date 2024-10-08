#include "World.h"

// ��� Actor
void UWorld::AddActor(std::shared_ptr<AActor> Actor) {
    ActiveGroupActors.insert(Actor);
}

// �Ƴ� Actor
void UWorld::RemoveActor(std::shared_ptr<AActor> Actor) {
    ActiveGroupActors.erase(Actor);
}

// ÿ֡�������� Actor
void UWorld::Tick(float DeltaTime) {
    for (const auto& Actor : ActiveGroupActors) {
        if (Actor) {
            Actor->Tick(DeltaTime); // ���� AActor ����һ�� Tick ����
        }
    }
}

// ��Ⱦ���� Actor
void UWorld::Render() {
    for (const auto& Actor : ActiveGroupActors) {
        if (Actor) {
            Actor->Render(); // ���� AActor ����һ�� Render ����
        }
    }
}
