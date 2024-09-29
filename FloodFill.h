#pragma once
#include "InputStateMachine.h"

namespace InputStates {
	class FloodFill :public InputState
	{
	public:
		FloodFill(Window& window) :InputState(window){};
		// ͨ�� InputState �̳�
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;
		void Draw() override;

	private:
		std::vector<std::pair<int, int>> fillPos;
	};
}