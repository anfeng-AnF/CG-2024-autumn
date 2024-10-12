#include "UConvexComponent.h"

UConvexComponent::UConvexComponent()
{
}

UConvexComponent::~UConvexComponent()
{
}

void UConvexComponent::SetConvexVertices(const std::vector<DirectX::XMFLOAT3>& Vertices)
{
}

const std::vector<DirectX::XMFLOAT3>& UConvexComponent::GetConvexVertices() const
{
	return {};
}

FHitResult UConvexComponent::CheckCollision(const UCollisionComponent& Other) const
{
	return FHitResult();
}
