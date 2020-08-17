#include "App.h"
#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Math.h"
#include "Sheet.h"
#include "SkinnedCube.h"
#include "Cylinder.h"
#include "AssimpObject.h"

#include <memory>
#include <algorithm>
#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

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
	const auto transform = DirectX::XMMatrixRotationRollPitchYaw( pos.roll, pos.pitch, pos.yaw ) *
		DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
	nanosuit.Draw( wnd.Gfx(), transform );
	light.Draw( wnd.Gfx() );

	// imgui
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();
	
	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if ( ImGui::Begin( "Model", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Orientation" ) )
		{
			ImGui::SliderAngle( "Roll", &pos.roll, -180.0f, 180.0f );
			ImGui::SliderAngle( "Pitch", &pos.pitch, -180.0f, 180.0f );
			ImGui::SliderAngle( "Yaw", &pos.yaw, -180.0f, 180.0f );
		}

		if ( ImGui::CollapsingHeader( "Position" ) )
		{
			ImGui::SliderFloat( "X", &pos.x, -20.0f, 20.0f );
			ImGui::SliderFloat( "Y", &pos.y, -20.0f, 20.0f );
			ImGui::SliderFloat( "Z", &pos.z, -20.0f, 20.0f );
		}
	}
	ImGui::End();
}