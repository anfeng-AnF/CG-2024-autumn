#include "ImguiManager.h"


ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Font\\hk4e_zh-cn.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}

void ImguiManager::SetScale(float Scale)
{
	ImGui::GetIO().FontGlobalScale = Scale;
}
