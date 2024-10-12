#include "USphereComponent.h"

USphereComponent::USphereComponent()
{
}

USphereComponent::~USphereComponent()
{
}

void USphereComponent::SetSphereRadius(float Radius)
{
}

const DirectX::BoundingSphere& USphereComponent::GetBoundingSphere() const
{
	return {};
}

FHitResult USphereComponent::CheckCollision(const UCollisionComponent& Other) const
{
	return FHitResult();
}
