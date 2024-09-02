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
	ctrl(&cam, wnd.Gfx()),
	threadPool(10),
	arrow(wnd.Gfx(),"Models\\arrow\\Position.fbx")
{
	wnd.Gfx().SetProjection(Perspective);
	wnd.DisableCursor();

	auto a=Sphere::Make(4.0f);
	//ctrl.AddGeomerty(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices));
	//ctrl.AddGeomerty(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices,DirectX::XMFLOAT3{ 10.0f,0.0f,0.0f }));
	//ctrl.AddGeomerty(std::make_shared<TriangelGeo>(wnd.Gfx(), a.vertices, a.indices,DirectX::XMFLOAT3{ 0.0f,10.0f,0.0f }));

	Dvtx::VertexBuffer vbuf(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ 100.0f,0.0f,100.0f });
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ -100.0f,0.0f,100.0f });
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ -100.0f,0.0f,-100.0f });
	vbuf.EmplaceBack(DirectX::XMFLOAT3{ 100.0f,0.0f,-100.0f });
	std::vector<uint16_t> ind = { 3,0,1,3,1,2 };

	//ctrl.AddGeomerty(wnd.Gfx(),vbuf,ind);

	Dvtx::VertexBuffer vbufLine(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
	float radius = 1.0f; // 圆弧的半径
	float startAngle = 0.0f; // 起始角度（弧度制）
	float endAngle = DirectX::XM_PI * 2; // 终止角度（弧度制），这里是90度
	int numSegments = 20; // 圆弧分割成的段数
	ind.clear();
	for (int i = 0; i <= numSegments; ++i)
	{
		float theta = startAngle + (endAngle - startAngle) * (i / static_cast<float>(numSegments));
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		vbufLine.EmplaceBack(DirectX::XMFLOAT3(x, 0.0f, z)); // 假设Y轴为0，即XZ平面上的圆弧
		ind.push_back(i);
		ind.push_back(i+1);
	}
	ind.resize(numSegments * 2);
	ctrl.AddGeomerty(std::make_shared<Line>(wnd.Gfx(), vbufLine, ind));
	//ctrl.AddGeomerty(Arrow::ArrowConstruceHelper(wnd.Gfx()));
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


	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());
	arrow.Draw(wnd.Gfx());
	//nano.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	axis.Draw(wnd.Gfx());


		if (!wnd.mouse.RightIsPressed())
		{
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
		}
		else
		{
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
		}


	if (!wnd.CursorEnabled())
	{
		if (wnd.Kbd.KeyIsPressed('W') && isPerspective)
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.Kbd.KeyIsPressed('A'))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
		if (wnd.Kbd.KeyIsPressed('S') && isPerspective)
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.Kbd.KeyIsPressed('D'))
		{
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.Kbd.KeyIsPressed(VK_SPACE)) {
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.Kbd.KeyIsPressed(VK_CONTROL)) {
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.Kbd.KeyIsPressed('Q'))
		{
			cam.RotateRoll(1.0f);
		}
		if (wnd.Kbd.KeyIsPressed('E'))
		{
			cam.RotateRoll(-1.0f);
		}
	}


	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.RotatePitchYaw((float)delta->x, (float)delta->y);
		}
	}
	while (const auto delta = wnd.mouse.Read())
	{
		if (delta->GetType() == Mouse::Event::Type::LPress) 
		{
			std::pair<int,int>pos=delta->GetPos();
			std::ostringstream oss;
			oss << "Position: (" << pos.first << ", " << pos.second << ")"<<std::endl;
			OutputDebugStringA(oss.str().c_str());
			if (ctrl.SelectGeomerty(pos.first, pos.second, width, height,isPerspective)) {
				//ctrl.TransformGeomerty(wnd);
			}
		}
		if (wnd.CursorEnabled())break;
		if (delta->GetType() == Mouse::Event::Type::WheelDown)
		{
			cam.DecreaseTravelSpeed();
		}
		if (delta->GetType() == Mouse::Event::Type::WheelUp)
		{
			cam.IncreaseTravelSpeed();
		}
	}
	ctrl.Draw();
	// imgui windows
	ImGui::Begin("Menu");
	ImGui::Checkbox("Use Perspective", &isPerspective);
	ctrl.TransformGeomerty(wnd);
	ImGui::End();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	//nano.ShowWindow();
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




