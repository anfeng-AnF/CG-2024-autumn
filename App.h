#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"



class App
{
public:
	App() = delete;
	App(float width,float height);
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
	float width, height;
};