#include "MyAnimInstance.h"
#include "functional"

MyAnimInstance::MyAnimInstance(SkeletonMesh& SkMesh):
	Super(SkMesh)
{
	auto vAnimAssert = AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\elysiaAnim1.fbx");
	const int BeginRun = 2;
	const int Deceleration = 3;
	const int Idle = 4;
	const int Run = 6;

	auto animStateMachine = std::make_shared<AnimStateMachine>();
	animStateMachine->AddState("BeginRun", std::make_shared<AnimAsset>(vAnimAssert[BeginRun]));
	animStateMachine->AddState("Deceleration", std::make_shared<AnimAsset>(vAnimAssert[Deceleration]),0.2f);
	animStateMachine->AddState("Idle", std::make_shared<AnimAsset>(vAnimAssert[Idle]),0.8f);
	animStateMachine->AddState("Run", std::make_shared<AnimAsset>(vAnimAssert[Run]),1.2f);
	animStateMachine->SetCurrentState("Idle");

	animStateMachine->SetTransitionCondition("Idle", "Run", [this]() {
		return currentState == State::Run;
		},
		0.2f
	);
	animStateMachine->SetTransitionCondition("Run", "Deceleration", [this]() {
		return currentState == State::Deceleration;
		},
		0.2f
	);
	animStateMachine->SetTransitionCondition("Deceleration", "Idle", [this]() {
		return currentState == State::Idle;
		},
		0.2f
	);
	animStateMachine->SetTransitionCondition("Deceleration", "Run", [this]() {
		return currentState == State::Run;
		},
		0.02f
	);
	StateMachine = animStateMachine;
}

#include "imguiManager.h"

void MyAnimInstance::Update(float DeltaTime)
{
	static float LastFrameSpeed = 0.0f;
	currentTime += DeltaTime;
	if (StateMachine) {
		StateMachine->Update(DeltaTime);
		currentPose = StateMachine->GetCurrentPose();
	}
	mesh.SetBonesTransform(currentPose);

	//¼õËÙ
	if (LastFrameSpeed > this->Info.CurrentGroundSpeed) {
		if (this->Info.CurrentGroundSpeed < 10.f&& this->Info.CurrentGroundSpeed>1.f) {
			currentState = Deceleration;
		}
		else if (this->Info.CurrentGroundSpeed >= 10.f) {
			currentState = Run;
		}
		else
		{
			currentState = Idle;
		}
	}
	else//¼ÓËÙ
	{
		if (this->Info.CurrentGroundSpeed > 0.0f) {
			currentState = Run;
		}
		else
		{
			currentState = Idle;
		}
	}
	LastFrameSpeed = this->Info.CurrentGroundSpeed;
}

void MyAnimInstance::SetAnimInfo(AnimInfo Speed)
{
	this->Info = Speed;
}
