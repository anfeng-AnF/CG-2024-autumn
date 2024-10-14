#include "MyAnimInstance.h"
#include "functional"

MyAnimInstance::MyAnimInstance(SkeletonMesh& SkMesh):
	Super(SkMesh)
{
	auto vAnimAssert = AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\elysiaAnim1.fbx");
	const int BeginRun = 1;
	const int Idle = 2;
	const int Run = 6;

	auto animStateMachine = std::make_shared<AnimStateMachine>();
	animStateMachine->AddState("BeginRun", std::make_shared<AnimAsset>(vAnimAssert[BeginRun]));
	animStateMachine->AddState("Idle", std::make_shared<AnimAsset>(vAnimAssert[Idle]),0.8f);
	animStateMachine->AddState("Run", std::make_shared<AnimAsset>(vAnimAssert[Run]),1.2f);
	animStateMachine->SetCurrentState("Idle");

	animStateMachine->SetTransitionCondition("Idle", "Run", [this]() {
		return currentState == State::Run;
		},
		0.2f
	);
	animStateMachine->SetTransitionCondition("Run", "Idle", [this]() {
		return currentState == State::Idle;
		},
		0.2f
	);

	StateMachine = animStateMachine;
}

#include "imguiManager.h"

void MyAnimInstance::Update(float DeltaTime)
{
	currentTime += DeltaTime;
	if (StateMachine) {
		StateMachine->Update(DeltaTime);
		currentPose = StateMachine->GetCurrentPose();
	}
	mesh.SetBonesTransform(currentPose);
	if (this->Info.CurrentGroundSpeed > 4.0f) {
		currentState = Run;
	}
	else
	{
		currentState = Idle;
	}
}

void MyAnimInstance::SetAnimInfo(AnimInfo Speed)
{
	this->Info = Speed;
}
