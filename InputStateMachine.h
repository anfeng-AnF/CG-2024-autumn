#pragma once
#include "Window.h"
#include <unordered_map>
#include <memory>
#include <string>

#define PERVIOUS_STATE "pervious"
#define DEFAULT_STATE  "default"
class InputStateMachine;

class InputState {
    friend class InputStateMachine;
public:
    explicit InputState(Window& window) : wnd(window) {}

    virtual void Enter() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Exit() = 0;
    virtual void Draw() = 0;
    virtual ~InputState() = default;
protected:
    Window& wnd;
    InputStateMachine* Machine;
};

class InputStateMachine
{
public:
    InputStateMachine() {};

    void DoFrame(float deltaTime) {
        if (currentState) {
            currentState->Draw();
            currentState->Update(deltaTime);
        }
    }

    void AddState(const std::string& name, std::unique_ptr<InputState> state) {
        state->Machine = this;
        states[name] = std::move(state);
    }

    void SetState(const std::string& name) {
        auto it = states.find(name);
        if (it == states.end()&&name!=PERVIOUS_STATE) {
            throw std::runtime_error("State not found: "+name);
            return;
        }

        if (currentState) {
            currentState->Exit();
        }

        if (name == PERVIOUS_STATE) {
            std::swap(currentState, perviousState);
        }
        else
        {
            perviousState = currentState;
            currentState = it->second.get();
        }

        if (currentState) {
            currentState->Enter();
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<InputState>> states;
    InputState* currentState = nullptr;
    InputState* perviousState = nullptr;
};
