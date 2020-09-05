#include "App.h"

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

	{
		{
			auto pass = std::make_unique<BufferClearPass>( "clear" );
			pass->SetInputSource( "renderTarget","$.backbuffer" );
			pass->SetInputSource( "depthStencil","$.masterDepth" );
			rg.AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<LambertianPass>( wnd.Gfx(),"lambertian" );
			pass->SetInputSource( "renderTarget","clear.renderTarget" );
			pass->SetInputSource( "depthStencil","clear.depthStencil" );
			rg.AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineMaskPass>( wnd.Gfx(),"outlineMask" );
			pass->SetInputSource( "depthStencil","lambertian.depthStencil" );
			rg.AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineDrawPass>( wnd.Gfx(),"outlineDraw" );
			pass->SetInputSource( "renderTarget","lambertian.renderTarget" );
			pass->SetInputSource( "depthStencil","outlineMask.depthStencil" );
			rg.AppendPass( std::move( pass ) );
		}
		rg.SetSinkTarget( "backbuffer","outlineDraw.renderTarget" );
		rg.Finalize();

		cube.LinkTechniques( rg );
		cube2.LinkTechniques( rg );
		light.LinkTechniques( rg );
		//sponza.LinkTechniques( rg );
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
	//sponza.Submit();
	//goblin.Submit();
	//backpack.Submit();
	cube.Submit();
	cube2.Submit();

	rg.Execute( wnd.Gfx() );

	// imgui
	if ( wnd.Gfx().IsImGuiEnabled() )
	{
		static MP modelProbe;
		camera.SpawnControlWindow();
		light.SpawnControlWindow();
		//modelProbe.SpawnWindow( sponza );
		cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );
		cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );
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