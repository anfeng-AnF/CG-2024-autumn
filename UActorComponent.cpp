#include "UActorComponent.h"

std::shared_ptr<AActor> UActorComponent::GetOwner() const
{
    return this->Owner;
}
