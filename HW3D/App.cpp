#include "App.h"
#include "Camera.h"
#include "Channels.h"

#include "ModelProbeWindow.h"

#include "BufferClearPass.h"
#include "LambertianPass.h"

#include "OutlineDrawPass.h"
#include "OutlineMaskPass.h"

#include <memory>
#include <algorithm>

App::App( const std::string& commandLine ) :
	wnd( 1280, 720, "DirectX 11 Engine Window" ),
	light( wnd.Gfx(), { 10.0f, 5.0f, 0.0f } ),
	light1( wnd.Gfx(), { 24.25f, 6.5f, 11.0f } ),
	light2( wnd.Gfx(), { 24.25f, 6.5f, -7.0f } ),
	light3( wnd.Gfx(), { -31.0f, 6.5f, 11.0f } ),
	light4( wnd.Gfx(), { -31.0f, 6.5f, -7.0f } ),
	scriptCommander( TokenizeQuoted( commandLine ) )
{
	cameras.AddCamera( std::make_unique<Camera>( wnd.Gfx(), "A", DirectX::XMFLOAT3{ -13.5f, 6.0f, 3.5f }, 0.0f, PI / 2.0f ) );
	cameras.AddCamera( std::make_unique<Camera>( wnd.Gfx(), "B", DirectX::XMFLOAT3{ -13.5f, 28.8f, -6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f ) );
	cameras.AddCamera( light.ShareCamera() );
	
	cube.SetPos( { 10.0f, 5.0f, 6.0f } );
	cube2.SetPos( { 10.0f, 5.0f, 14.0f } );
	nanosuit.SetRootTransform(
		DirectX::XMMatrixRotationY( PI / 2.0f ) *
		DirectX::XMMatrixTranslation( 27.0f, -0.56f, 1.7f )
	);
	goblin.SetRootTransform(
		DirectX::XMMatrixRotationY( -PI / 2.0f ) *
		DirectX::XMMatrixTranslation( -8.0f, 10.0f, 0.0f )
	);
	backpack.SetRootTransform(
		DirectX::XMMatrixRotationY( PI / 1.0f ) *
		DirectX::XMMatrixTranslation( 10.0f, 5.0f, -8.0f )
	);

	cube.LinkTechniques( rg );
	cube2.LinkTechniques( rg );
	light.LinkTechniques( rg );
	sponza.LinkTechniques( rg );
	nanosuit.LinkTechniques( rg );
	goblin.LinkTechniques( rg );
	backpack.LinkTechniques( rg );
	cameras.LinkTechniques( rg );
	
	light1.LinkTechniques( rg );
	light2.LinkTechniques( rg );
	light3.LinkTechniques( rg );
	light4.LinkTechniques( rg );

	rg.BindShadowCamera( *light.ShareCamera() );
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
	if ( wnd.kbd.KeyIsPressed( VK_ESCAPE ) )
		wnd.EndWindow();

	// imgui setup
	if ( wnd.kbd.KeyIsPressed( VK_F1 ) )
		wnd.Gfx().DisableImGui();
	else
		wnd.Gfx().EnableImGui();
	
	// raw mouse input
	while ( const auto& e = wnd.kbd.ReadKey() )
	{
		if ( !e->IsPress() )
			continue;

		switch ( e->GetCode() )
		{
		case VK_INSERT:
			if ( wnd.CursorEnabled() )
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
		case VK_NUMPAD1:
			saveDepth = true;
			break;
		}
	}

	// camera movement
	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed( 'W' ) )
			cameras->Translate( { 0.0f, 0.0f, dt } );
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
			cameras->Translate( { -dt, 0.0f, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
			cameras->Translate( { 0.0f, 0.0f, -dt } );
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
			cameras->Translate( { dt, 0.0f, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
			cameras->Translate( { 0.0f, dt, 0.0f } );
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
			cameras->Translate( { 0.0f, -dt, 0.0f } );
	}

	// camera rotation
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
			cameras->Rotate(delta->x, delta->y);
	}
}

void App::DoFrame( float dt )
{
	// setup
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	light.Bind( wnd.Gfx(), cameras->GetMatrix() );
	rg.BindMainCamera( cameras.GetActiveCamera() );

	// objects
	light.Submit( Channel::main );
	sponza.Submit( Channel::main );
	cameras.Submit( Channel::main );
	if ( loadLight1 )	light1.Submit( Channel::main );
	if ( loadLight2 )	light2.Submit( Channel::main );
	if ( loadLight3 )	light3.Submit( Channel::main );
	if ( loadLight4 )	light4.Submit( Channel::main );
	if ( loadNanosuit ) nanosuit.Submit( Channel::main );
	if ( loadGoblin )	goblin.Submit( Channel::main );
	if ( loadBackpack ) backpack.Submit( Channel::main );
	if ( loadCube1 )	cube.Submit( Channel::main );
	if ( loadCube2 )	cube2.Submit( Channel::main );

	light.Submit( Channel::shadow );
	sponza.Submit( Channel::shadow );
	if ( loadNanosuit ) nanosuit.Submit( Channel::shadow );
	if ( loadGoblin )	goblin.Submit( Channel::shadow );
	if ( loadBackpack ) backpack.Submit( Channel::shadow );
	if ( loadCube1 )	cube.Submit( Channel::shadow );
	if ( loadCube2 )	cube2.Submit( Channel::shadow );

	rg.Execute( wnd.Gfx() );

	if ( saveDepth )
	{
		rg.StoreDepth( wnd.Gfx(), "res\\depth\\view-depth.png" );
		saveDepth = false;
	}

	// imgui
	if ( wnd.Gfx().IsImGuiEnabled() )
	{
		static MP sponzaProbe;
		static MP nanosuitProbe;
		static MP goblinProbe;
		static MP backpackProbe;

		if ( ImGui::Begin( "Master Window", FALSE, ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 0.0f, 1.0f });
			if ( ImGui::TreeNode( "Models" ) )
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });

				ImGui::Checkbox( "Sponza", &loadSponza );
				if ( loadSponza ) sponzaProbe.SpawnWindow( sponza, "Sponza" );

				ImGui::Checkbox( "Nanosuit", &loadNanosuit );
				if ( loadNanosuit ) nanosuitProbe.SpawnWindow( nanosuit, "Nanosuit" );

				ImGui::Checkbox( "Goblin", &loadGoblin );
				if ( loadGoblin ) goblinProbe.SpawnWindow( goblin, "Goblin" );

				ImGui::Checkbox( "Backpack", &loadBackpack );
				if ( loadBackpack ) backpackProbe.SpawnWindow( backpack, "Backpack" );

				ImGui::PopStyleColor();
				ImGui::TreePop();
			}
			ImGui::PopStyleColor();

			ImGui::PushStyleColor( ImGuiCol_Text, { 0.0f, 1.0f, 1.0f, 1.0f } );
			if ( ImGui::TreeNode( "Objects" ) )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );

				ImGui::Checkbox( "Cameras", &loadCameras );
				if ( loadCameras ) cameras.SpawnControlWindow( wnd.Gfx() );

				ImGui::Checkbox( "Cube 1", &loadCube1 );
				if ( loadCube1 ) cube.SpawnControlWindow( wnd.Gfx(), "Cube 1" );

				ImGui::Checkbox( "Cube 2", &loadCube2 );
				if ( loadCube2 ) cube2.SpawnControlWindow( wnd.Gfx(), "Cube 2" );

				ImGui::PopStyleColor();
				ImGui::TreePop();
			}
			ImGui::PopStyleColor();

			ImGui::PushStyleColor( ImGuiCol_Text, { 0.5f, 1.0f, 0.5f, 1.0f } );
			if ( ImGui::TreeNode( "Lights" ) )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );

				ImGui::Checkbox( "Main Light", &loadLight );
				if ( loadLight ) light.SpawnControlWindow( "Main Light" );

				ImGui::Checkbox( "Sub Light1", &loadLight1 );
				if ( loadLight1 ) light1.SpawnControlWindow( "Sub Light1" );

				ImGui::Checkbox( "Sub Light2", &loadLight2 );
				if ( loadLight2 ) light2.SpawnControlWindow( "Sub Light2" );

				ImGui::Checkbox( "Sub Light3", &loadLight3 );
				if ( loadLight3 ) light3.SpawnControlWindow( "Sub Light3" );

				ImGui::Checkbox( "Sub Light4", &loadLight4 );
				if ( loadLight4 ) light4.SpawnControlWindow( "Sub Light4" );

				ImGui::PopStyleColor();
				ImGui::TreePop();
			}
			ImGui::PopStyleColor();
			
			ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 0.0f, 1.0f, 1.0f } );
			if ( ImGui::TreeNode( "Render Graph" ) )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f } );

				ImGui::Checkbox( "Blurring", &loadBlur );
				if ( loadBlur ) rg.RenderKernelWindow( wnd.Gfx() );

				ImGui::Checkbox( "Raw Input", &loadRaw );
				if ( loadRaw ) ShowRawInputWindow();

				ImGui::PopStyleColor();
				ImGui::TreePop();
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
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