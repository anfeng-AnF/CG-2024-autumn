#include "App.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "GDIPlusManager.h"
#include "AssTest.h"
#include "VertexLayout.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
GDIPlusManager gdipm;
namespace dx = DirectX;

void f()
{
	VertexBuffer vb(std::move(
		VertexLayout{}
		.Append<VertexLayout::Position3D>()
		.Append<VertexLayout::Normal>()
		.Append<VertexLayout::Texture2D>()
	));
	vb.EmplaceBack(
		dx::XMFLOAT3{ 1.0f,1.0f,5.0f },
		dx::XMFLOAT3{ 2.0f,1.0f,4.0f },
		dx::XMFLOAT2{ 6.0f,9.0f }
	);
	vb.EmplaceBack(
		dx::XMFLOAT3{ 6.0f,9.0f,6.0f },
		dx::XMFLOAT3{ 9.0f,6.0f,9.0f },
		dx::XMFLOAT2{ 4.2f,0.0f }
	);
	auto pos = vb[0].Attr<VertexLayout::Position3D>();
	auto nor = vb[0].Attr<VertexLayout::Normal>();
	auto tex = vb[1].Attr<VertexLayout::Texture2D>();
	vb.Back().Attr<VertexLayout::Position3D>().z = 420.0f;
	pos = vb.Back().Attr<VertexLayout::Position3D>();
}

App::App(float width, float height) :wnd(width, height, L"∏ ”Í"), width(width), height(height), light(wnd.Gfx())
{

	Assimp::Importer imp;

	f();


	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = {1.0,1.0,1.0 };
			//return std::make_unique<Box>(
			//	gfx, rng, adist, ddist,
			//	odist, rdist, bdist, mat
			//);
			return std::make_unique<AssTest>(
				gfx, rng, adist, ddist,
				odist, rdist, mat, 1.5f
			);
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };		
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, height/width, 0.5f, 1000));
	wnd.Gfx().SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
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
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	for (auto& b : drawables)
	{
		b->Update(wnd.Kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		b->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	// imgui stuff
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static char buffer[1024];

	if (ImGui::Begin("simulation speed")) {
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 6.0f, "%.4f");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::InputText("Butts", buffer, sizeof(buffer));
		ImGui::End();
	}

	// imgui window to control camera
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// present

	wnd.Gfx().EndFrame();
}
