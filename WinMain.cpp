#include "App.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cwchar>

void CreateConsole() {
	AllocConsole();  // 分配一个新的控制台
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);  // 将标准输出重定向到控制台
	freopen_s(&file, "CONOUT$", "w", stderr);  // 将标准错误重定向到控制台
	freopen_s(&file, "CONIN$", "r", stdin);    // 将标准输入重定向到控制台
}

wchar_t* charToWchar(const char* charArray) {
	// 获取转换后的宽字符串长度
	size_t len;
	mbstowcs_s(&len, nullptr, 0, charArray, 0);
	len += 1; // 包括终止符

	// 分配内存
	wchar_t* wCharArray = new wchar_t[len];

	// 执行实际转换
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
		return App{}.Go();
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, charToWchar(e.what()), charToWchar(e.GetType()), MB_OK | MB_ICONEXCLAMATION);
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

