#include "App.h"
#include "Math.h"
#include "NormalMapTweak.h"

#include <memory>
#include <algorithm>
#include <sstream>
#include <shellapi.h>

#include "imgui/imgui.h"

#include "Surface.h"
#include "GDIPlusManager.h"
GDIPlusManager gdipm;

App::App( const std::string& commandLine ) : wnd( 1280, 720, "DirectX 11 Engine Window" ), light( wnd.Gfx() )
{
	if ( this->commandLine != "" )
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW( pLineW, &nArgs );
		if ( nArgs >= 4 && std::wstring( pArgs[1] ) == L"--ntwerk-rotx180" )
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			NormalMapTweak::RotateXAxis180(
				std::string( pathInWide.begin(), pathInWide.end() ),
				std::string( pathOutWide.begin(), pathOutWide.end() )
			);
		}
	}
	
	goblin.SetRootTransform( DirectX::XMMatrixTranslation( 0.0f, 0.0f, -4.0f ) );
	nanosuit.SetRootTransform( DirectX::XMMatrixTranslation( 0.0f, -7.0f, 6.0f ) );
	wall.SetRootTransform( DirectX::XMMatrixTranslation( -12.0f, 0.0f, 0.0f ) );
	plane.SetPos( { 12.0f, 0.0f, 0.0f } );
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
	const auto dt = timer.Mark();

	if ( wnd.kbd.KeyIsPressed( VK_ESCAPE ) )
		wnd.EndWindow();

	// imgui setup
	if ( wnd.kbd.KeyIsPressed( VK_F1 ) )
		wnd.Gfx().DisableImGui();
	else
		wnd.Gfx().EnableImGui();

	// setup
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( camera.GetMatrix() );
	light.Bind( wnd.Gfx(), camera.GetMatrix() );

	// objects
	goblin.Draw( wnd.Gfx() );
	nanosuit.Draw( wnd.Gfx() );
	wall.Draw( wnd.Gfx() );
	plane.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );

	// raw mouse input
	while ( const auto& e = wnd.kbd.ReadKey() )
	{
		if ( !e->IsPress() )
			continue;

		switch ( e->GetCode() )
		{
		case VK_INSERT:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		}
	}

	// camera movement
	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed( 'W' ) )
			camera.Translate( { 0.0f, 0.0f, dt } );
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
			camera.Translate( { -dt, 0.0f, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
			camera.Translate( { 0.0f, 0.0f, -dt } );
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
			camera.Translate( { dt, 0.0f, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
			camera.Translate( { 0.0f, dt, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
			camera.Translate( { 0.0f, -dt, 0.0f } );
	}

	// camera rotation
	while ( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if ( !wnd.CursorEnabled() )
			camera.Rotate( delta->x, delta->y );
	}

	// imgui
	if ( wnd.Gfx().IsImGuiEnabled() )
	{
		camera.SpawnControlWindow();
		light.SpawnControlWindow();
		goblin.ShowControlWindow( wnd.Gfx(), "Goblin" );
		nanosuit.ShowControlWindow( wnd.Gfx(), "Nanosuit" );
		wall.ShowControlWindow( wnd.Gfx(), "Wall" );
		plane.SpawnControlWindow( wnd.Gfx() );
		ShowRawInputWindow();
	}
	
	wnd.Gfx().EndFrame();
}

void App::ShowRawInputWindow()
{
	while ( const auto d = wnd.mouse.ReadRawDelta() )
	{
		x += d->x;
		y += d->y;
	}
	if ( ImGui::Begin( "Raw Input" ) )
	{
		ImGui::Text( "Tally: (%d,%d)", x, y );
		ImGui::Text( "Cursor: %s", wnd.CursorEnabled() ? "Enabled" : "Disabled" );
	}
	ImGui::End();
}