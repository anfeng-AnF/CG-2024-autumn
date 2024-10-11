#include "MyGameMode.h"

MyGameMode::MyGameMode(Window& wnd, InputStateMachine& ISM):
	ISM(ISM),
	Wnd(Wnd)
{
	DefaultCharacter = std::make_shared<ACharacter>();
	DefaultWorld->AddActor(DefaultCharacter);
	ISM.AddState("Game", std::make_unique<GameInput>(wnd, DefaultCharacter));
}

MyGameMode::~MyGameMode()
{
}

void MyGameMode::Begin()
{
	ISM.SetState("Game");
}

void MyGameMode::End()
{
	ISM.SetState(DEFAULT_STATE);
}

void MyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Wnd.Gfx().SetCamera(DefaultCharacter->GetCamera());
}
