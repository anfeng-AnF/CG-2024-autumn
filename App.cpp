#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "imgui/imgui.h"
#include "GDIPlusManager.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "Sphere.h"
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
	Anim(elysia)
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
	ISM.AddState("CameraMove", std::make_unique<Camera::CameraMove>(cam.inputState));
	ISM.AddState("SpawnGeo", std::make_unique<SpawnGeometryByInput::SpawnGeoInputState>(SG.inputState));
	ISM.SetState(DEFAULT_STATE);

	//Anim.currentAnim = AnimAsset::ReadAnimAssertFromFile("Models\\Lantern\\LanternAnim.fbx")[0];
	Anim.currentAnim = AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\elysiaAnim.fbx")[0];
	//Anim.currentAnim = AnimAsset::ReadAnimAssertFromFile("Models\\skeletonMeshs\\SkeletonMeshTestAnim.fbx")[0];
	//auto aaaa = AnimAsset::ReadAnimAssertFromFile("Models\\Elysia\\\\elysiaAnim.fbx")[0];
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
	float deltaTime = timer.Mark();
	//first set Projection matrix
	if (isPerspective) {
		wnd.Gfx().SetProjection(Perspective);
	}
	else
	{
		wnd.Gfx().SetProjection(Orthographic);
	}

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());
	//Lantern.Draw(wnd.Gfx());
	if (wnd.Kbd.KeyIsPressed(VK_F1)) {
		Anim.Update(deltaTime);
	}
	elysia.CtrlWnd(wnd.Gfx());
	elysia.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	//skeletonMesh.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	axis.Draw(wnd.Gfx());

	ISM.DoFrame(deltaTime);
	DGM.Draw(wnd.Gfx());
	Bind::Codex::DebugString();
	//ctrl.Draw(wnd.Gfx());
	// imgui windows
	ImGui::Begin("Menu");
	ImGui::Checkbox("Use Perspective", &isPerspective);
	const char* proj = "current: Perspective";
	if (!isPerspective)proj = "current: Orthographic";
	ImGui::Text(proj);
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
	ImGui::Text("FPS: %.2f", fps);
	frame++;

	ctrl.DrawImGui(wnd.Gfx());
	ImGui::End();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	//Lantern.ShowWindow();
	//wall.ShowWindow();
	Codex::DebugString();
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




