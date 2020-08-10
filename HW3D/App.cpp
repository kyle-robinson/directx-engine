#include "App.h"
#include "Box.h"
#include <memory>
//#include <sstream>
//#include <iomanip>

App::App() : wnd( 800, 600, "DirectX 11 Engine Window" )
{
	std::mt19937 rng(std::random_device{}() );
	std::uniform_real_distribution<float> adist( 0.0f, 3.1415f * 2.0f );
	std::uniform_real_distribution<float> ddist( 0.0f, 3.1415f * 2.0f );
	std::uniform_real_distribution<float> odist( 0.0f, 3.1415f * 0.3f );
	std::uniform_real_distribution<float> rdist( 6.0f, 20.0f );
	for ( auto i = 0; i < 80; i++ )
	{
		boxes.push_back(
			std::make_unique<Box>(
				wnd.Gfx(), rng, adist, ddist, odist, rdist
			)
		);
	}
	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 40.0f ) );
}

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

App::~App() { }

void App::DoFrame()
{
	//const float c = sin( timer.Peek() ) / 2.0f + 0.5f;
	//wnd.Gfx().ClearBuffer( c, c, 1.0f );
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer( 0.07f, 0.0f, 0.12f );

	for ( auto& b : boxes )
	{
		b->Update( dt );
		b->Draw( wnd.Gfx() );
	}

	/*wnd.Gfx().DrawTriangle(
		timer.Peek(),
		wnd.mouse.GetPosX() / 400.0f - 1.0f,
		-wnd.mouse.GetPosY() / 300.0f + 1.0f
	);*/

	/*wnd.Gfx().DrawCube(
		timer.Peek(),
		0.0f,
		0.0f
	);

	wnd.Gfx().DrawCube(
		-timer.Peek(),
		wnd.mouse.GetPosX() / 400.0f - 1.0f,
		-wnd.mouse.GetPosY() / 300.0f + 1.0f
	);*/
	
	wnd.Gfx().EndFrame();
	
	/*const float t = timer.Peek();
	std::wostringstream oss;
	oss << "Timer elapsed: " << std::setprecision( 1 ) << std::fixed << t << "s";
	wnd.SetTitle( oss.str() );
	if (timer.Peek() >= 10.0f)
		exit(1);*/
}