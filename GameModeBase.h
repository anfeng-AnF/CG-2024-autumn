#pragma once
#include <DirectXMath.h>
#include <memory>
#include "InputStateMachine.h"
#include "ACharacter.h"
#include "World.h"
#include "Graphics.h"

class GameInput :public InputState
{
public:
    GameInput(Window& wnd, std::shared_ptr<ACharacter> playerCharacter) :InputState(wnd),playerCharacter(std::move(playerCharacter)) {}
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
    virtual void Tick(float DeltaTime);
    virtual void Begin();
    virtual void End();
    virtual void Render(Graphics& Gfx);
protected:
    std::shared_ptr<ACharacter> DefaultCharacter;
    std::shared_ptr<UWorld> DefaultWorld;
private:

};

