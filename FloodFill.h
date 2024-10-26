#pragma once
#include "InputStateMachine.h"

namespace InputStates {
	class FloodFill :public InputState
	{
	public:
		enum {
			FloodFillMode,
			DrawLineMidpoint,
			DrawLineBresenham,
			ClipMode,
		} OperatingMode = FloodFillMode;

		struct ScreenPos {
			int x, y;
		};

	public:
		FloodFill(Window& window) :InputState(window) { ResetClipWindow(); };
		// Í¨¹ý InputState ¼Ì³Ð
		void Enter() override;
		void Update(float deltaTime) override;
		void Exit() override;
		void Draw() override;

	private:
		void HandleMouseMsg(std::optional<Mouse::Event>& msg);
		void ResetClipWindow();
	private:
		std::vector<std::pair<int, int>> fillPos;
		std::vector<std::pair<ScreenPos, ScreenPos>> Lines;
		std::pair<ScreenPos, ScreenPos> ClipWindow;
		std::vector<int> OperationCounter= std::vector<int>(5, 0);				//OperatingMode OperatingCount
	};
}