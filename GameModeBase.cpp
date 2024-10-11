#include "GameModeBase.h"

void GameInput::Enter()
{
	wnd.Kbd.Flush();
	wnd.mouse.Flush();
	wnd.DisableCursor();
	wnd.mouse.EnableRaw();
}

void GameInput::Update(float deltaTime)
{
    //Handle keyboard input
    int x = 0, y = 0;
    while (auto c = wnd.Kbd.ReadKey())
    {
        if (c.value().GetCode() == 'w') {
            x = 1;
        }
        else if (c.value().GetCode() == 's') {
            x = -1;
        }
        else if(c.value().GetCode() == 'd')
        {
            y = 1;
        }
        else if (c.value().GetCode() == 'a')
        {
            y = -1;
        }
        else if (c.value().GetCode() == VK_SPACE)
        {
            playerCharacter->Jump();
        }
    }
    playerCharacter->MoveInput(x, y);
    //Handling of mouse input
    while (auto RawDelta = wnd.mouse.ReadRawDelta())
    {
        playerCharacter->AddYawInput(RawDelta.value().x);
        playerCharacter->AddPitchInput(RawDelta.value().y);
    }

}

void GameInput::Exit()
{

}

void GameInput::Draw()
{

}


GameModeBase::GameModeBase()
{
}

GameModeBase::~GameModeBase()
{
}

void GameModeBase::Tick(float DeltaTime)
{
    DefaultWorld->Tick(DeltaTime);
}

void GameModeBase::Begin()
{
}

void GameModeBase::End()
{
}