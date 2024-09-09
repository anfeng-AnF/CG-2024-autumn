#include "InputStateMachine.h"
InputStateMachine* InputStateMachine::ISM = nullptr;

InputStateMachine& InputStateMachine::GetInstance(Mouse& m, Keyboard& k, Window& w)
{
    if (ISM == nullptr) {
        ISM = new InputStateMachine(m, k, w);
    }
    return *ISM;
}

InputStateMachine& InputStateMachine::GetInstance()
{
    assert(ISM&&"instance without init");
    return *ISM;
}

void InputStateMachine::DoFrame()
{
    auto deltaTime = timer.Mark();
    state->HandleKeyboardInput();
    state->HandleMouseInput();
    state->Update(deltaTime);
}

InputStateMachine::InputStateMachine(Mouse& m, Keyboard& k, Window& w):
    mouse(m),
    kbd(k),
    wnd(w)
{
    state = new InputStateClass::Default(wnd);
}

void InputStateMachine::ChangeState(InputStateClass::InputState* newInputState)
{
    if (state) {
        state->Exit();
    }
    state = newInputState;
    if (state) {
        state->Enter();
    }
}
