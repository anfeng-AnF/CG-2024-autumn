#include "MyCharacter.h"
#include "USkeletalMeshComponent.h"
#include "MyAnimInstance.h"
#include "USpringArmComponent.h"

MyCharacter::MyCharacter(Graphics& gfx)
{
	auto SkMesh = std::make_shared<USkeletalMeshComponent>();
	SkMesh->RelationTransform.rotation = XMQuaternionRotationRollPitchYaw(XM_PI,0,0);

	SkMesh->SetSkeletalMesh(std::make_shared<SkeletonMesh>(gfx, "Models\\Elysia\\elysiaQ2.fbx"));

	auto AnimInst = std::make_shared<MyAnimInstance>(*SkMesh->GetSkeletalMesh().get());

	SkMesh->SetAnimationInstance(AnimInst);
	AddComponent(AActor::RootComponentName,SkMesh,USkeletalMeshComponent::name);
	Components[USpringArmComponent::name]->ActorComponent->RelationTransform.position = XMVectorSet(0.0f, 5.f, 0.0f, 0.0f);
}

MyCharacter::~MyCharacter()
{
}

void MyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (auto skMeshComp = static_cast<USkeletalMeshComponent*>(Components[USkeletalMeshComponent::name]->ActorComponent.get())) {
		if (auto myAnimInstance = static_cast<MyAnimInstance*>(skMeshComp->GetAnimationInstance().get())) {
			myAnimInstance->SetAnimInfo({
				XMVectorGetX(XMVector3Length(CharacterMovementComponent->Velocity)),
				CharacterMovementComponent->maxGroundSpeed
				});
		}
	}
}
