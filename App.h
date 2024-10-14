#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Axis.h"
#include "Mesh.h"
#include <set>
#include "CtrlCeomerty.h"
#include "ThreadPool.h"
#include "CtrlableGeometry.h"
#include "InputStateMachine.h"
#include "DebugGraphsMannger.h"
#include "SpawnGeometryByInput.h"
#include "SkeletonMesh.h"
#include "AnimInstance.h"
#include "MyGameMode.h"

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
	bool isPerspective = true;
	bool showDemoWindow = false;
	ThreadPool threadPool;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 1;
	UINT width, height;
	float speed_factor = 1.0f;
	Camera cam;	
	PointLight light;	
	CollisionGeoManager ctrl;
	//StaticMesh wall{ wnd.Gfx(),"Models\\RockWall\\RockWall.fbx" };
	//StaticMesh Lantern{ wnd.Gfx(),"Models\\Lantern\\Lantern_Fixed1.fbx" };

	SkeletonMesh elysia{ wnd.Gfx(),"Models\\Elysia\\elysiaQ2.fbx" };

	AnimInstance Anim;
	Axis axis{ wnd.Gfx() };
	InputStateMachine ISM;
	DebugGraphsMannger &DGM=DebugGraphsMannger::GetInstence();
	SpawnGeometryByInput SG;

	MyGameMode Game;

};