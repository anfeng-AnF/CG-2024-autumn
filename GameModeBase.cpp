#include "GameModeBase.h"
void GameInput::Enter()
{
	wnd.DisableCursor();
	wnd.Kbd.Flush();
	wnd.mouse.Flush();
	wnd.mouse.EnableRaw();
}

void GameInput::Update(float deltaTime)
{
    //Handle keyboard input
    int x = 0, y = 0;
    while (auto c = wnd.Kbd.ReadKey())
    {
        if (c.value().GetCode() == VK_SPACE&&c.value().IsPress())
        {
            playerCharacter->Jump();
        }
        else if (c.value().GetCode() == VK_MENU && c.value().IsRelease()) {
            wnd.DisableCursor();
        }
    }
    if (wnd.Kbd.KeyIsPressed(VK_MENU)) {
        wnd.EnableCursor();
    }
    else if (wnd.Kbd.KeyIsPressed('w')) {
        x = 1;
    }
    else if (wnd.Kbd.KeyIsPressed('s')) {
        x = -1;
    }
    else if (wnd.Kbd.KeyIsPressed('d'))
    {
        y = 1;
    }
    else if (wnd.Kbd.KeyIsPressed('a'))
    {
        y = -1;
    }

    //playerCharacter->MoveInput(x, y);
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

void GameModeBase::Render(Graphics& Gfx)
{
    this->DefaultWorld->Render(Gfx);
}
