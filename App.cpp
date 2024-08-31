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


App::App(UINT width, UINT height)
	:
	wnd(width, height, L"����"),
	width(width), height(height), light(wnd.Gfx()),
	ctrl(&cam, wnd.Gfx()),
	threadPool(10)
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, height/ (float)width, 0.5f, 100000));
	wnd.DisableCursor();

	auto a=Sphere::Make(4.0f);
	ctrl.AddGeomerty(wnd.Gfx(), a.vertices, a.indices);
	ctrl.AddGeomerty(wnd.Gfx(), a.vertices, a.indices,{10.0f,0.0f,0.0f});
	ctrl.AddGeomerty(wnd.Gfx(), a.vertices, a.indices, { 0.0f,10.0f,0.0f });
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
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	//nano.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	axis.Draw(wnd.Gfx());

	while (const auto e = wnd.Kbd.ReadKey())
	{
		if (e->IsPress() && e->GetCode() == VK_MENU)
		{
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
		}
		else if (e->IsRelease() && e->GetCode() == VK_MENU)
		{
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
		}
		
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.Kbd.KeyIsPressed('W'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.Kbd.KeyIsPressed('A'))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
		if (wnd.Kbd.KeyIsPressed('S'))
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
			if (ctrl.SelectGeomerty(pos.first, pos.second, width, height)) {
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




