#include "UActorComponent.h"

void UActorComponent::SetOwner(AActor* NewOwner)
{
    Owner = NewOwner;
}

DirectX::XMMATRIX UActorComponent::GetRelationTransformMatrix()
{
    return RelationTransform.GetMatrix();
}

AActor* UActorComponent::GetOwner() const
{
    return this->Owner;
}
