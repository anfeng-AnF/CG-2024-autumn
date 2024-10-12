#include "UCapsuleComponent.h"

const std::string UCapsuleComponent::name = "UCapsuleComponent";


UCapsuleComponent::UCapsuleComponent()
{
}

UCapsuleComponent::~UCapsuleComponent()
{
}

void UCapsuleComponent::SetCapsule(float Radius, float Height)
{
}

float UCapsuleComponent::GetRadius() const
{
    return 0.0f;
}

float UCapsuleComponent::GetHeight() const
{
    return 0.0f;
}

FHitResult UCapsuleComponent::CheckCollision(const UCollisionComponent& Other) const
{
    return FHitResult();
}

DirectX::BoundingBox UCapsuleComponent::GetBoundingBox() const
{
    return DirectX::BoundingBox();
}

DirectX::BoundingSphere UCapsuleComponent::GetBoundingSphere() const
{
    return DirectX::BoundingSphere();
}

void UCapsuleComponent::Update(float DeltaTime)
{
}
