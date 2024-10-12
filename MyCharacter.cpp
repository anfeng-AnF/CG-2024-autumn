#include "MyCharacter.h"
#include "USkeletalMeshComponent.h"
#include "MyAnimInstance.h"
#include "USpringArmComponent.h"

MyCharacter::MyCharacter(Graphics& gfx)
{
	auto SkMesh = std::make_shared<USkeletalMeshComponent>();
	SkMesh->RelationTransform.rotation = XMQuaternionRotationRollPitchYaw(XM_PI,0,0);

	auto vAnimAssert = AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\elysiaAnim.fbx");
	SkMesh->SetSkeletalMesh(std::make_shared<SkeletonMesh>(gfx, "Models\\Elysia\\elysiaQ2.fbx"));

	auto AnimInst = std::make_shared<MyAnimInstance>(*SkMesh->GetSkeletalMesh().get());
	AnimInst->SetCurrentAnim(vAnimAssert[0]);
	SkMesh->SetAnimationInstance(AnimInst);
	AddComponent(AActor::RootComponentName,SkMesh,USkeletalMeshComponent::name);
	Components[USpringArmComponent::name]->ActorComponent->RelationTransform.position = XMVectorSet(0.0f, 5.f, 0.0f, 0.0f);
}

MyCharacter::~MyCharacter()
{
}
