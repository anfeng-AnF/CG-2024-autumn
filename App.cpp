#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "GDIPlusManager.h"
#include "Vertex.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
GDIPlusManager gdipm;
namespace dx = DirectX;


App::App(UINT width, UINT height) 
	:
	wnd(width, height, L"����"), 
	width(width), height(height), light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, height/ (float)width, 0.5f, 100000));
	wnd.DisableCursor();

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

	nano.Draw(wnd.Gfx());
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
		if (wnd.Kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.Kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.0f,0.0f,dt });
		}
	}


	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.RotatePitchYaw(delta->x, delta->y);
		}
	}

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	nano.ShowWindow();


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


