#include "App.h"
#include "Math.h"
#include "imgui/imgui.h"

#include <memory>
#include <algorithm>

App::App() : wnd( 1280, 720, "DirectX 11 Engine Window" ), light( wnd.Gfx() )
{
	cube.SetPos( { 4.0f, 0.0f, 0.0f } );
	cube2.SetPos( { 0.0f, 4.0f, 0.0f } );

	{
		std::string path = "res\\models\\brick_wall\\brick_wall.obj";
		Assimp::Importer importer;
		const auto pScene = importer.ReadFile( path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);
		Material mat{ wnd.Gfx(), *pScene->mMaterials[1], path };
		pLoaded = std::make_unique<Mesh>( wnd.Gfx(), mat, *pScene->mMeshes[0] );
	}

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
	light.Submit( fc );
	//cube.Submit( fc );
	//cube2.Submit( fc );
	pLoaded->Submit( fc, DirectX::XMMatrixIdentity() );

	fc.Execute( wnd.Gfx() );

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
		cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );
		cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );
		ShowRawInputWindow();
	}
	
	wnd.Gfx().EndFrame();
	fc.Reset();
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