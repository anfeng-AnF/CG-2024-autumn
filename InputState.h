#pragma once
#include "Window.h"

namespace InputStateClass {
	class InputState {
	public:
		explicit InputState(Window& window) : wnd(window) {}

		virtual void Enter() = 0;
		virtual void HandleKeyboardInput() = 0;
		virtual void HandleMouseInput() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Exit() = 0;
		virtual ~InputState() = default;
	protected:
		Window& wnd;
	};

	class Default :public InputState
	{
	public:
		Default(Window& wnd) :InputState(wnd) {};

		// Í¨¹ý InputState ¼Ì³Ð
		void Enter() override;
		void HandleKeyboardInput() override;
		void HandleMouseInput() override;
		void Update(float deltaTime) override;
		void Exit() override;
	};
}