#include "App.h"
#include <sstream>
#include <iomanip>

App::App() : wnd( 800, 600, "DirectX 11 Engine Window" ) { }

int App::Init()
{
	// handle messages
	while ( true )
	{
		if ( const auto ecode = Window::ProcessMessages() )
			return *ecode;

		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin( timer.Peek() ) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer( c, c, 1.0f );

	/*wnd.Gfx().DrawTriangle(
		timer.Peek(),
		wnd.mouse.GetPosX() / 400.0f - 1.0f,
		-wnd.mouse.GetPosY() / 300.0f + 1.0f
	);*/

	wnd.Gfx().DrawCube(
		timer.Peek(),
		0.0f,
		0.0f
	);

	wnd.Gfx().DrawCube(
		timer.Peek(),
		wnd.mouse.GetPosX() / 400.0f - 1.0f,
		-wnd.mouse.GetPosY() / 300.0f + 1.0f
	);
	
	wnd.Gfx().EndFrame();
	
	/*const float t = timer.Peek();
	std::wostringstream oss;
	oss << "Timer elapsed: " << std::setprecision( 1 ) << std::fixed << t << "s";
	wnd.SetTitle( oss.str() );
	if (timer.Peek() >= 10.0f)
		exit(1);*/
}