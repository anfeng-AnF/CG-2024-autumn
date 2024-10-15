#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "imgui/imgui.h"
#include "GDIPlusManager.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "Sphere.h"
#include "FloodFill.h"
#include <unordered_set>
GDIPlusManager gdipm;
namespace dx = DirectX;

#define Perspective XMMatrixPerspectiveLH(1.0f, height / (float)width, 0.5f, 100000)
#define Orthographic XMMatrixOrthographicLH(30.0f, 30.0f* height / (float)width, 0.1f, 100000)

App::App(UINT width, UINT height)
	:
	wnd(width, height, L"甘雨"),
	width(width), height(height), light(wnd.Gfx()),
	threadPool(10),
	cam(wnd),
	ctrl(wnd,cam),
	SG(wnd,cam,&ctrl),
	Anim(elysia),
	Game(wnd,ISM)
{
	wnd.Gfx().SetProjection(Perspective);
	wnd.DisableCursor();

	auto a=Sphere::Make(2.0f);
	/*
	ctrl.AddGeometry(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices, DirectX::XMFLOAT3{20.0f,0.0f,0.0f}));
	ctrl.AddGeometry(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices, DirectX::XMFLOAT3{ 10.0f,0.0f,0.0f }));
	ctrl.AddGeometry(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices, DirectX::XMFLOAT3{ 0.0f,10.0f,20.0f }));
	*/
	Dvtx::VertexBuffer vbuf(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ 100.0f,0.0f,100.0f });
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ -100.0f,0.0f,100.0f });
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ -100.0f,0.0f,-100.0f });
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ 100.0f,0.0f,-100.0f });
	std::vector<uint16_t> ind = { 3,0,1,3,1,2 };

	//ctrl.AddGeometry(wnd.Gfx(),vbuf,ind);

	Dvtx::VertexBuffer vbufLine(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
	float radius=5.0f;
	int segmentCount=20;
	float angleStep = DirectX::XM_2PI / segmentCount;
	ind.clear();
	for (int i = 0; i < segmentCount; ++i)
	{
		float angle = i * angleStep;

		// 计算当前段的起点
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		vbufLine.EmplaceBack(DirectX::XMFLOAT3(x, y, 0.0f));
	}

	for (int i = 0; i < segmentCount-12; ++i)
	{
		int prevIndex = (i - 1 + segmentCount) % segmentCount;
		int currIndex = i;
		int nextIndex = (i + 1) % segmentCount;
		int nextNextIndex = (i + 2) % segmentCount;

		// 添加 4 个索引：前一个顶点，当前顶点，下一个顶点，再下一个顶点
		ind.push_back(prevIndex);
		ind.push_back(currIndex);
		ind.push_back(nextIndex);
		ind.push_back(nextNextIndex);
	}
	//ctrl.AddGeometry(std::make_shared<WidthLine>(wnd.Gfx(), cam, vbufLine, ind));

	std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> b = CreateCircleWithAdjacency({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,10.0f },{0.0f,1.0f,0.0f});
	ctrl.AddGeometry(std::make_shared<WidthLine>(wnd.Gfx(), cam, b.first, b.second));

	std::pair<Dvtx::VertexBuffer, std::vector<uint16_t>> c = CreateLineWithAdjacency({ 10.0f,0.0f,0.0f }, { 0.0f,0.0f,10.0f });
	ctrl.AddGeometry(std::make_shared<WidthLine>(wnd.Gfx(), cam, c.first, c.second));


	ISM.AddState(DEFAULT_STATE, std::make_unique<CollisionGeoManager::TranslationState>(ctrl.inputState));
	ISM.AddState("CameraMove", std::move(cam.inputState));
	ISM.AddState("SpawnGeo", std::make_unique<SpawnGeometryByInput::SpawnGeoInputState>(SG.inputState));
	ISM.AddState("FloodFill", std::make_unique<InputStates::FloodFill>(wnd));
	ISM.SetState(DEFAULT_STATE);

	//Anim.currentAnim = AnimAsset::ReadAnimAssertFromFile("Models\\Lantern\\LanternAnim.fbx")[0];
	Anim.SetCurrentAnim(AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\elysiaAnim1.fbx")[4]);
	//Anim.currentAnim = AnimAsset::ReadAnimAssertFromFile("Models\\GLInstance\\dancing_vampire.dae")[0];
	//Anim.currentAnim = AnimAsset::ReadAnimAssertFromFile("Models\\skeletonMeshs\\SkeletonMeshTestAnim.fbx")[0];
	//Lantern.SetShader<Bind::PixelShader>("Phong2DPS.cso", "");
	//Lantern.SetShader<Bind::VertexShader>("Phong2DVS.cso", "");
}
int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) {

			return *ecode;
		}
		DoFrame();
	}
	return 0;
}

App::~App()
{
}

void App::DoFrame()
{
	static bool StartGame = 0;
	float deltaTime = timer.Mark();
	//first set Projection matrix

	if (isPerspective) {
		wnd.Gfx().SetProjection(Perspective);
	}
	else
	{
		wnd.Gfx().SetProjection(Orthographic);
	}

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);//Default background color
	if (!StartGame) {
		wnd.Gfx().SetCamera(cam.GetMatrix());
		light.Bind(wnd.Gfx(), cam.GetMatrix());
		//Lantern.Draw(wnd.Gfx());
		Anim.Update(deltaTime);
		elysia.CtrlWnd(wnd.Gfx());
		elysia.Draw(wnd.Gfx(),DirectX::XMMatrixRotationX(XM_PI));
		//wall.Draw(wnd.Gfx());
		//skeletonMesh.Draw(wnd.Gfx());
		//light.Draw(wnd.Gfx());
		axis.Draw(wnd.Gfx());
		ctrl.Draw(wnd.Gfx());
		DGM.Draw(wnd.Gfx());
	}
	else
	{
		//axis.Draw(wnd.Gfx());
		Game.Tick(deltaTime);
		Game.Render(wnd.Gfx());
	}
	ISM.DoFrame(deltaTime);

	// imgui windows
	ImGui::Begin("Menu");
	bool ModeChanged = ImGui::Checkbox("Play Game", &StartGame);
	
	if (StartGame&&ModeChanged) {
		Game.Begin();
	}
	else if (!StartGame && ModeChanged) {
		Game.End();
	}
	if (!StartGame) {
		ImGui::Checkbox("Use Perspective", &isPerspective);
		const char* proj = "current: Perspective";
		if (!isPerspective)proj = "current: Orthographic";
		ImGui::Text(proj);
	}
	//FPS
	static int frame = 0;
	static const int framesPerUpdate = 20;  // 每 20 帧更新一次
	static float accumulatedTime = 0.0f;    // 累积的时间
	accumulatedTime += deltaTime;
	static float fps = 0;
	if (frame % framesPerUpdate == 0) {
		fps = framesPerUpdate / accumulatedTime;  // 计算 FPS
		accumulatedTime = 0.0f;  // 重置时间
	}
	static float runTime = 0;
	ImGui::Text("FPS: %.2f", fps);
	ImGui::Text("runtime %.0f", runTime+=deltaTime);
	frame++;

	static bool UsePhong = false;
	bool shaderChange = ImGui::Checkbox("Use Phong Shader", &UsePhong);

	if (!StartGame) {
		ctrl.DrawImGui(wnd.Gfx());
	}
	ImGui::End();
	if (!StartGame) {
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		ShowImguiDemoWindow();
	}

	//Lantern.ShowWindow();

	if (shaderChange) {
		if (UsePhong)
		{
			elysia.SetShader<Bind::PixelShader>("SkeletonMeshPhongPS.cso", "SkeletonMeshPS.cso");
		}
		else
		{
			elysia.SetShader<Bind::PixelShader>("SkeletonMeshPS.cso","SkeletonMeshPhongPS.cso");
		}
	}

	Codex::DebugString();

	//reprocess

		//wnd.Gfx().PostProcessingOnCPU(f,1080/2,640/2,0xff0000ff);

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}




