#include "App.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cwchar>

void CreateConsole() {
	AllocConsole();  // ����һ���µĿ���̨
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);  // ����׼����ض��򵽿���̨
	freopen_s(&file, "CONOUT$", "w", stderr);  // ����׼�����ض��򵽿���̨
	freopen_s(&file, "CONIN$", "r", stdin);    // ����׼�����ض��򵽿���̨
}

wchar_t* charToWchar(const char* charArray) {
	// ��ȡת����Ŀ��ַ�������
	size_t len;
	mbstowcs_s(&len, nullptr, 0, charArray, 0);
	len += 1; // ������ֹ��

	// �����ڴ�
	wchar_t* wCharArray = new wchar_t[len];

	// ִ��ʵ��ת��
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

