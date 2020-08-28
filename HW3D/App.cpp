#include "App.h"
#include "Math.h"
#include "imgui/imgui.h"

#include <memory>
#include <algorithm>

App::App() : wnd( 1280, 720, "DirectX 11 Engine Window" ), light( wnd.Gfx() )
{
	/*if ( this->commandLine != "" )
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW( pLineW, &nArgs );
		if ( nArgs >= 3 && std::wstring( pArgs[1] ) == L"--twerk-objnorm" )
		{
			const std::wstring pathInWide = pArgs[2];
			TexturePreprocessor::FlipAllYNormalsInObj(
				std::string( pathInWide.begin(), pathInWide.end() )
			);
		}
		else if ( nArgs >= 3 && std::wstring( pArgs[1] ) == L"--twerk-flipy" )
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			TexturePreprocessor::FlipYNormalMap(
				std::string( pathInWide.begin(), pathInWide.end() ),
				std::string( pathOutWide.begin(), pathOutWide.end() )
			);
		}
		else if ( nArgs >= 4 && std::wstring( pArgs[1] ) == L"--twerk-validate" )
		{
			const std::wstring minWide = pArgs[2];
			const std::wstring maxWide = pArgs[3];
			const std::wstring pathWide = pArgs[4];
			TexturePreprocessor::ValidateNormalMap(
				std::string( pathWide.begin(), pathWide.end() ), std::stof( minWide ), std::stof( maxWide )
			);
		}
	}*/
	
	//goblin.SetRootTransform( DirectX::XMMatrixTranslation( 0.0f, 0.0f, -4.0f ) );
	//nanosuit.SetRootTransform( DirectX::XMMatrixTranslation( 0.0f, -7.0f, 6.0f ) );
	//wall.SetRootTransform( DirectX::XMMatrixTranslation( -12.0f, 0.0f, 0.0f ) );
	//plane.SetPos( { 12.0f, 0.0f, 0.0f } );
	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 400.0f ) );
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
	//goblin.Draw( wnd.Gfx() );
	//nanosuit.Draw( wnd.Gfx() );
	//wall.Draw( wnd.Gfx() );
	//plane.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );
	sponza.Draw( wnd.Gfx() );

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
		sponza.ShowControlWindow( wnd.Gfx() );
		//goblin.ShowControlWindow( wnd.Gfx(), "Goblin" );
		//nanosuit.ShowControlWindow( wnd.Gfx(), "Nanosuit" );
		//wall.ShowControlWindow( wnd.Gfx(), "Wall" );
		//plane.SpawnControlWindow( wnd.Gfx() );
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