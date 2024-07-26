#include "App.h"

App::App():wnd(1080,640,L"∏ ”Í")
{

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

void App::DoFrame()
{
	const float r = sin(timer.Peek()+3.14/3);
	const float g = sin(timer.Peek()+3.14/4);
	const float b = sin(timer.Peek());
	wnd.Gfx().ClearBuffer(r,g,b);
	wnd.Gfx().EndFrame();
}
