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
		{
			return *ecode;
		}

		Update();
	}
}

void App::Update()
{
	const float t = timer.Peek();
	std::wostringstream oss;
	oss << "Timer elapsed: " << std::setprecision( 1 ) << std::fixed << t << "s";
	wnd.SetTitle( oss.str() );
}