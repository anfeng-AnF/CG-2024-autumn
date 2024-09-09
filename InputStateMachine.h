#pragma once
#include "Mouse.h"
#include "Keyboard.h"
#include "Window.h"
#include "InputState.h"


class InputStateMachine
{
public:
	static InputStateMachine& GetInstance(Mouse& m, Keyboard& k, Window& w);
	static InputStateMachine& GetInstance();
	void DoFrame();

private:
	InputStateMachine(Mouse&m,Keyboard&k,Window&w);
	InputStateMachine(const InputStateMachine&) = delete;
	InputStateMachine operator=(const InputStateMachine&) = delete;
	void ChangeState(InputStateClass::InputState* newInputState);
private:
	Mouse& mouse;
	Keyboard& kbd;
	Window& wnd;
	ChiliTimer timer;
	InputStateClass::InputState* state;
	static InputStateMachine* ISM;
};
