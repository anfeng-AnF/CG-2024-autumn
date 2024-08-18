#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
//#include "Axis.h"
#include "Mesh.h"
#include <set>



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
	void ShowImguiDemoWindow();

private:	
	bool showDemoWindow = false;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 1;
	UINT width, height;
	float speed_factor = 1.0f;
	Camera cam;	
	PointLight light;	
	Model nano{ wnd.Gfx(),"Models\\Lantern\\Lantern_Fixed1.fbx" };
	Model nano2{ wnd.Gfx(),"Models\\Lantern\\Lantern_Fixed1.fbx" };
	//Axis axis{ wnd.Gfx() };
};