#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cwchar>
#include "App.h"

void CreateConsole() {
	AllocConsole();  // ����һ���µĿ���̨
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);  
	freopen_s(&file, "CONOUT$", "w", stderr);  
	freopen_s(&file, "CONIN$", "r", stdin);    
}

wchar_t* charToWchar(const char* charArray) {
	size_t len;
	mbstowcs_s(&len, nullptr, 0, charArray, 0);
	len += 1;

	wchar_t* wCharArray = new wchar_t[len];

	mbstowcs_s(&len, wCharArray, len, charArray, len - 1);

	return wCharArray;
}

using namespace DirectX;
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow)
{
	try
	{
		if (SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) == FALSE) {
			// ���ʧ�ܣ�����ʹ�þɵ� SetProcessDPIAware API
			SetProcessDPIAware();
		}
		int screenWidth = GetSystemMetrics(SM_CXSCREEN)*0.75;
		int screenHeight = GetSystemMetrics(SM_CYSCREEN)*0.75;

		return App(screenWidth, screenHeight).Go();
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, charToWchar(e.what()), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}

