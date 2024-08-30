#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "ChiliTimer.h"
#include "Graphics.h"
#include <sdkddkver.h>
#include <Windows.h>
#include <optional>
#include <memory>
#include <string>



class Window
{
private:
	// singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const WCHAR* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const WCHAR* wndClassName = L"my window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(UINT width, UINT height, const WCHAR* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::wstring& title);
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();	
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;

private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;

	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	bool cursorEnabled = false;
	int width;
	int height;
	HWND hWnd;
	std::vector<BYTE> rawBuffer;

public:
	Keyboard Kbd;
	Mouse mouse;
	std::unique_ptr<Graphics> pGfx;
};