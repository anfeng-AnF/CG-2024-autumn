#pragma once
#include <DirectXMath.h>
#include <memory>
#include "InputStateMachine.h"
#include "ACharacter.h"
class GameInput :public InputState
{
public:
    GameInput(Window& wnd) :InputState(wnd) {}
    // Í¨¹ý InputState ¼Ì³Ð
    void Enter() override;
    void Update(float deltaTime) override;
    void Exit() override;
    void Draw() override;

public:
    std::shared_ptr<ACharacter> playerCharacter;

};

class GameModeBase
{
public:
	GameModeBase();
	~GameModeBase();

protected:
    std::shared_ptr<ACharacter> PlayerCharacter;

private:

};

GameModeBase::GameModeBase()
{
}

GameModeBase::~GameModeBase()
{
}