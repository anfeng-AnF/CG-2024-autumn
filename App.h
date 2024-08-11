#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"



class App
{
public:
	App() = delete;
	App(UINT width, UINT height);
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
	static constexpr size_t nDrawables = 1;
	UINT width, height;
	float speed_factor = 1.0f;
	Camera cam;	
	PointLight light;
};