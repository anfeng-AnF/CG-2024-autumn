#pragma once
#include "InputStateMachine.h"

namespace InputStates {
	class FloodFill :public InputState
	{
	public:
		FloodFill(Window& window) :InputState(window){};
		// Í¨¹ý InputState ¼Ì³Ð
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;
		void Draw() override;

	private:
		std::vector<std::pair<int, int>> fillPos;
	};
}