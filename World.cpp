#include "World.h"
#include "UCollisionComponent.h"

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
            Actor->Tick(DeltaTime);
        }
    }
}

// ��Ⱦ���� Actor
void UWorld::Render(Graphics& Gfx) {
    for (const auto& Actor : ActiveGroupActors) {
        if (Actor) {
            Actor->Render(Gfx);
        }
    }
}

std::vector<FCollisionResult> UWorld::QuaryCollision(AActor* Actor)
{
    return {};
}
