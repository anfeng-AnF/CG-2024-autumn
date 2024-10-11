#pragma once
#include "GameModeBase.h"

class MyGameMode : public GameModeBase
{
	typedef GameModeBase Super;
public:
	MyGameMode(Window& wnd, InputStateMachine& ISM);
	~MyGameMode();
	void Begin()override;
	void End()override;
	void Tick(float DeltaTime)override;
private:
	Window& Wnd;
	InputStateMachine& ISM;
};