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
    while (auto e = wnd.Kbd.ReadKey())
    {
        if (e.value().GetCode() == VK_SPACE && e.value().IsPress())
        {
            playerCharacter->Jump();
        }
        if (e->IsPress() && e->GetCode() == VK_MENU)
        {
            wnd.EnableCursor();
            wnd.mouse.DisableRaw();
        }
        if (e->IsRelease() && e->GetCode() == VK_MENU)
        {
            wnd.DisableCursor();
            wnd.mouse.EnableRaw();
        }
    }
    if (wnd.Kbd.KeyIsPressed('W')) {
        x += -1;
    }
    if (wnd.Kbd.KeyIsPressed('S')) {
        x += 1;
    }
    if (wnd.Kbd.KeyIsPressed('D'))
    {
        y += 1;
    }
    if (wnd.Kbd.KeyIsPressed('A'))
    {
        y += -1;
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

void GameModeBase::Render(Graphics& Gfx)
{
    this->DefaultWorld->Render(Gfx);
}
