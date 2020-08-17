#include "App.h"
#include "Math.h"
#include "AssimpObject.h"

#include <memory>
#include <algorithm>
#include <sstream>

#include "imgui/imgui.h"

#include "Surface.h"
#include "GDIPlusManager.h"
GDIPlusManager gdipm;

App::App() : wnd( 1000, 800, "DirectX 11 Engine Window" ), light( wnd.Gfx() )
{
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
	const auto dt = timer.Mark() * speed_factor;

	// imgui setup
	if ( wnd.kbd.KeyIsPressed( VK_F2 ) )
		wnd.Gfx().DisableImGui();
	else
		wnd.Gfx().EnableImGui();

	// setup
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( camera.GetMatrix() );
	light.Bind( wnd.Gfx(), camera.GetMatrix() );

	// objects
	nanosuit.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );

	// imgui
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	nanosuit.ShowControlWindow();
	
	wnd.Gfx().EndFrame();
}