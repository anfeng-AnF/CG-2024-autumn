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
	ctrl(wnd,cam)
{
	wnd.Gfx().SetProjection(Perspective);
	wnd.DisableCursor();

	auto a=Sphere::Make(2.0f);
	ctrl.AddGeometry(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices, DirectX::XMFLOAT3{ 20.0f,0.0f,0.0f }));
	ctrl.AddGeometry(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices, DirectX::XMFLOAT3{ 10.0f,0.0f,0.0f }));
	ctrl.AddGeometry(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices, DirectX::XMFLOAT3{ 0.0f,10.0f,20.0f }));

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
	ctrl.AddGeometry(std::make_shared<WidthLine>(wnd.Gfx(),cam, vbufLine, ind));//

	ISM.AddState(DEFAULT_STATE, std::make_unique<CollisionGeoManager::TranslationState>(ctrl.inputState));
	ISM.AddState("CameraMove", std::make_unique<Camera::CameraMove>(cam.inputState));
	ISM.SetState(DEFAULT_STATE);
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
	//wall.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	axis.Draw(wnd.Gfx());

	ISM.DoFrame(timer.Mark());
	DGM.Draw(wnd.Gfx());
//	static bool isUseCC;
//	if (!wnd.mouse.RightIsPressed()||isUseCC)
//	{
//		wnd.EnableCursor();
//		wnd.mouse.DisableRaw();
//	}
//	else
//	{
//		wnd.DisableCursor();
//		wnd.mouse.EnableRaw();
//	}
//
//
//	if (!wnd.CursorEnabled())
//	{
//		if (wnd.Kbd.KeyIsPressed('W') && isPerspective)
//		{
//			cam.Translate({ dt,0.0f,0.0f });
//		}
//		if (wnd.Kbd.KeyIsPressed('A'))
//		{
//			cam.Translate({ 0.0f,-dt,0.0f });
//		}
//		if (wnd.Kbd.KeyIsPressed('S') && isPerspective)
//		{
//			cam.Translate({ -dt,0.0f,0.0f });
//		}
//		if (wnd.Kbd.KeyIsPressed('D'))
//		{
//			cam.Translate({ 0.0f,dt,0.0f });
//		}
//		if (wnd.Kbd.KeyIsPressed(VK_SPACE)) {
//			cam.Translate({ 0.0f,0.0f,dt });
//		}
//		if (wnd.Kbd.KeyIsPressed(VK_CONTROL)) {
//			cam.Translate({ 0.0f,0.0f,-dt });
//		}
//		if (wnd.Kbd.KeyIsPressed('Q'))
//		{
//			cam.RotateRoll(1.0f);
//		}
//		if (wnd.Kbd.KeyIsPressed('E'))
//		{
//			cam.RotateRoll(-1.0f);
//		}
//	}
//	else
//	{
//		auto buffer = wnd.Kbd.ReadChar();
//		if (buffer.has_value()&&wnd.CursorEnabled()) {
//			ctrl.ChangeTransformationMethod(buffer.value());
//		}
//	}
//
//
//	std::pair<int, int> lastFarmPos;
//	while (const auto delta = wnd.mouse.ReadRawDelta())
//	{
//		if (!wnd.CursorEnabled()&&!isUseCC)
//		{
//			cam.RotatePitchYaw((float)delta->x, (float)delta->y);
//		}
//		if (isUseCC) {
//			lastFarmPos.first += delta->x;
//			lastFarmPos.second += delta->y;
//			wnd.UpdateMousePosition(delta->x, delta->y);
//		}
//	}
//	if (isUseCC) {
//		ctrl.OnTransformComponent(lastFarmPos,width,height);
//#ifdef DEBUG
//		std::ostringstream oss;
//		oss << lastFarmPos.first << "  " << lastFarmPos.second << std::endl;
//		OutputDebugStringA(oss.str().c_str());
//#endif // DEBUG
//	}
//	while (const auto delta = wnd.mouse.Read())
//	{
//		if (delta->GetType() == Mouse::Event::Type::LPress)
//		{
//			std::pair<int, int>pos = delta->GetPos();
//
//			if (ctrl.SelectGeomerty(pos.first, pos.second, width, height, isPerspective)) {
//				//ctrl.TransformGeomerty(wnd);
//				isUseCC = ctrl.IsUseCtrlComponent();
//			}
//		}
//		if (delta->GetType() == Mouse::Event::Type::LRelease) {
//			if (isUseCC) {
//				ctrl.EndComponentTransform();
//				isUseCC = false;
//			}
//		}
//		if (wnd.CursorEnabled())break;
//		if (delta->GetType() == Mouse::Event::Type::WheelDown)
//		{
//			cam.DecreaseTravelSpeed();
//		}
//		if (delta->GetType() == Mouse::Event::Type::WheelUp)
//		{
//			cam.IncreaseTravelSpeed();
//		}
//	}

	ctrl.Draw(wnd.Gfx());
	// imgui windows
	ImGui::Begin("Menu");
	ImGui::Checkbox("Use Perspective", &isPerspective);
	const char* proj = "current: Perspective";
	if (!isPerspective)proj = "current: Orthographic";
	ImGui::Text(proj);
	ctrl.DrawImGui(wnd.Gfx());
	ImGui::End();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	//Lantern.ShowWindow();
	//wall.ShowWindow();

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




