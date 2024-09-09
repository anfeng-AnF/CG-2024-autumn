#include "InputState.h"

void InputStateClass::Default::Enter()
{
}

void InputStateClass::Default::HandleKeyboardInput()
{
}

void InputStateClass::Default::HandleMouseInput()
{
	if (!wnd.mouse.RightIsPressed())
	{
		wnd.EnableCursor();
		wnd.mouse.DisableRaw();
	}
	else
	{
		wnd.DisableCursor();
		wnd.mouse.EnableRaw();
	}
}

void InputStateClass::Default::Update(float deltaTime)
{

}

void InputStateClass::Default::Exit()
{
}
