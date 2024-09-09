#include "Camera.h"

void InputStateClass::CameraMove::Enter()
{
}

void InputStateClass::CameraMove::HandleKeyboardInput()
{
}

void InputStateClass::CameraMove::HandleMouseInput()
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

void InputStateClass::CameraMove::Update(float deltaTime)
{
	if (!wnd.mouse.RightIsPressed()) {
		InputStateMachine::GetInstance()
	}
}

void InputStateClass::CameraMove::Exit()
{
}
