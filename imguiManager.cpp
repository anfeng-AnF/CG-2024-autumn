#include "ImguiManager.h"


ImguiManager::ImguiManager()
{
	ImGui::DebugCheckVersionAndDataLayout("1.69 WIP", sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert));
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
