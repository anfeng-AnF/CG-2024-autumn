#pragma once
#include "Window.h"
#include <unordered_map>
#include <memory>
#include <string>
class InputStateMachine;

class InputState {
    friend class InputStateMachine;
public:
    explicit InputState(Window& window) : wnd(window) {}

    virtual void Enter() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Exit() = 0;
    virtual ~InputState() = default;
protected:
    Window& wnd;
    InputStateMachine* Machine;
};

class InputStateMachine
{
public:
    InputStateMachine() = default;

    void DoFrame(float deltaTime) {
        if (currentState) {
            currentState->Update(deltaTime);
        }
    }

    void AddState(const std::string& name, std::unique_ptr<InputState> state) {
        state->Machine = this;
        states[name] = std::move(state);
    }

    void SetState(const std::string& name) {
        auto it = states.find(name);
        if (it == states.end()) {
            throw std::runtime_error("State not found: "+name);
            return;
        }

        if (currentState) {
            currentState->Exit();
        }
        currentState = it->second.get();
        if (currentState) {
            currentState->Enter();
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<InputState>> states;
    InputState* currentState = nullptr;
};
