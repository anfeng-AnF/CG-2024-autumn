#include "UActorComponent.h"

void UActorComponent::SetOwner(AActor* NewOwner)
{
    Owner = NewOwner;
}

AActor* UActorComponent::GetOwner() const
{
    return this->Owner;
}
