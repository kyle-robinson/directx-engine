#include "App.h"
#include "Math.h"

#include "ModelProbeWindow.h"

#include "BufferClearPass.h"
#include "LambertianPass.h"

#include "OutlineDrawPass.h"
#include "OutlineMaskPass.h"

#include <memory>
#include <algorithm>

App::App() : wnd( 1280, 720, "DirectX 11 Engine Window" ), light( wnd.Gfx() )
{
	cube.SetPos( { 4.0f, 0.0f, 0.0f } );
	cube2.SetPos( { -8.0f, 0.0f, 0.0f } );
	nanosuit.SetRootTransform(
		DirectX::XMMatrixRotationY( PI / 2.0f ) *
		DirectX::XMMatrixTranslation( 27.0f, -0.56f, 1.7f )
	);
	goblin.SetRootTransform(
		DirectX::XMMatrixRotationY( -PI / 2.0f ) *
		DirectX::XMMatrixTranslation( -8.0f, 10.0f, 0.0f )
	);
	backpack.SetRootTransform(
		DirectX::XMMatrixRotationY( PI / 0.5f ) *
		DirectX::XMMatrixTranslation( 10.0f, 5.0f, 10.0f )
	);

	cube.LinkTechniques( rg );
	cube2.LinkTechniques( rg );
	light.LinkTechniques( rg );
	sponza.LinkTechniques( rg );
	nanosuit.LinkTechniques( rg );
	goblin.LinkTechniques( rg );
	backpack.LinkTechniques( rg );

	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 400.0f ) );
}

int App::Init()
{
	// handle messages
	while ( true )
	{
		if ( const auto ecode = Window::ProcessMessages() )
			return *ecode;

		const auto dt = timer.Mark();
		HandleInput( dt );
		DoFrame( dt );
	}
}

App::~App() { }

void App::HandleInput( float dt )
{
	if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
		wnd.EndWindow();

	// imgui setup
	if (wnd.kbd.KeyIsPressed(VK_F1))
		wnd.Gfx().DisableImGui();
	else
		wnd.Gfx().EnableImGui();
	
	// raw mouse input
	while (const auto& e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
			continue;

		switch (e->GetCode())
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
	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
			camera.Translate({ 0.0f, 0.0f, dt });
		if (wnd.kbd.KeyIsPressed('A'))
			camera.Translate({ -dt, 0.0f, 0.0f });
		if (wnd.kbd.KeyIsPressed('S'))
			camera.Translate({ 0.0f, 0.0f, -dt });
		if (wnd.kbd.KeyIsPressed('D'))
			camera.Translate({ dt, 0.0f, 0.0f });
		if (wnd.kbd.KeyIsPressed('R'))
			camera.Translate({ 0.0f, dt, 0.0f });
		if (wnd.kbd.KeyIsPressed('F'))
			camera.Translate({ 0.0f, -dt, 0.0f });
	}

	// camera rotation
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
			camera.Rotate(delta->x, delta->y);
	}
}

void App::DoFrame( float dt )
{
	// setup
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( camera.GetMatrix() );
	light.Bind( wnd.Gfx(), camera.GetMatrix() );

	// objects
	light.Submit();
	sponza.Submit();
	nanosuit.Submit();
	goblin.Submit();
	backpack.Submit();
	cube.Submit();
	cube2.Submit();

	rg.Execute( wnd.Gfx() );

	// imgui
	if ( wnd.Gfx().IsImGuiEnabled() )
	{
		static MP sponzaProbe;
		static MP nanosuitProbe;
		static MP goblinProbe;
		static MP backpackProbe;

		sponzaProbe.SpawnWindow( sponza, "Sponza" );
		nanosuitProbe.SpawnWindow( nanosuit, "Nanosuit" );
		goblinProbe.SpawnWindow( goblin, "Goblin" );
		backpackProbe.SpawnWindow( backpack, "Backpack" );
		
		camera.SpawnControlWindow();
		light.SpawnControlWindow();
		cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );
		cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );
		rg.RenderWidgets( wnd.Gfx() );
		ShowRawInputWindow();
	}
	
	wnd.Gfx().EndFrame();
	rg.Reset();
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