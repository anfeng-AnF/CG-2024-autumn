#pragma once
#include "Mouse.h"
#include "Keyboard.h"
#include "Window.h"
class OperatorManager
{
public:
	enum Ops
	{
		Default,
		CameraMove,
		CtrlGeo
	};
public:
	static OperatorManager& GetInstance(Mouse& m, Keyboard& k,Window&w);
	void DoFrame();

private:
	OperatorManager(Mouse&m,Keyboard&k,Window&w);
	OperatorManager(const OperatorManager&) = delete;
	OperatorManager operator=(const OperatorManager&) = delete;

	void OnDefault();
	void OnCameraMove();
	void OnCtrlGeo();
private:
	Mouse& mouse;
	Keyboard& kbd;
	Window& wnd;

	Ops currentStatue=Ops::Default;
};