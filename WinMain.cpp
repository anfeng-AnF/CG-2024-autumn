#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cwchar>
#include "App.h"

void CreateConsole() {
	AllocConsole();  // 分配一个新的控制台
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

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow)
{
	try
	{
		return App(1080,720).Go();
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

