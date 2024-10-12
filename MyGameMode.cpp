#include "MyGameMode.h"
#include "MyCharacter.h"

MyGameMode::MyGameMode(Window& wnd, InputStateMachine& ISM):
	ISM(ISM),
	Wnd(wnd)
{
	DefaultWorld = std::make_shared<UWorld>();
	DefaultCharacter = std::make_shared<MyCharacter>(wnd.Gfx());
	DefaultWorld->AddActor(DefaultCharacter);
	ISM.AddState("Game", std::make_unique<GameInput>(wnd, DefaultCharacter));
}

MyGameMode::~MyGameMode()
{
}

void MyGameMode::Begin()
{
	Super::Begin();
	ISM.SetState("Game");
}

void MyGameMode::End()
{
	ISM.SetState(DEFAULT_STATE);
}

void MyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (DefaultCharacter) {
		auto matrix = DefaultCharacter->GetCameraMatrix();
		Wnd.Gfx().SetCamera(matrix);
	}
}
