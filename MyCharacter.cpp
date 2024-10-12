#include "MyCharacter.h"
#include "USkeletalMeshComponent.h"
#include "MyAnimInstance.h"


MyCharacter::MyCharacter(Graphics& gfx)
{
	auto SkMesh = std::make_shared<USkeletalMeshComponent>();

	auto vAnimAssert = AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\elysiaAnim.fbx");
	SkMesh->SetSkeletalMesh(std::make_shared<SkeletonMesh>(gfx, "Models\\Elysia\\elysiaQ2.fbx"));

	auto AnimInst = std::make_shared<MyAnimInstance>(*SkMesh->GetSkeletalMesh().get());
	AnimInst->SetCurrentAnim(vAnimAssert[0]);
	SkMesh->SetAnimationInstance(AnimInst);
	AddComponent(AActor::RootComponentName,SkMesh,USkeletalMeshComponent::name);
}

MyCharacter::~MyCharacter()
{
}
