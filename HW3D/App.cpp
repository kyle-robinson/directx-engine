#include "App.h"
#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Math.h"
#include "Sheet.h"
#include "SkinnedCube.h"
#include <memory>
#include <algorithm>
#include <sstream>

#include "res/imgui/imgui.h"
#include "res/imgui/imgui_impl_win32.h"
#include "res/imgui/imgui_impl_dx11.h"

#include "Surface.h"
#include "GDIPlusManager.h"
GDIPlusManager gdipm;

App::App() : wnd( 1000, 800, "DirectX 11 Engine Window" )
{
	class Factory
	{
	public:
		Factory( Graphics& gfx ) : gfx( gfx ) {}
		std::unique_ptr<Drawable> operator()()
		{
			switch ( typedist( rng ) )
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist, odist, rdist
				);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist, odist, rdist, bdist
				);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist, odist, rdist, longdist, latdist
				);
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist, odist, rdist
				);
			case 4:
				return std::make_unique<SkinnedCube>(
					gfx, rng, adist, ddist, odist, rdist
				);
			default:
				assert( false && "Bad drawable type in Factory!" );
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, PI * 1.0f };
		std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int> latdist{ 5, 20 };
		std::uniform_int_distribution<int> longdist{ 10, 40 };
		std::uniform_int_distribution<int> typedist{ 0, 4 };
	};

	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ), nDrawables, Factory( wnd.Gfx() ) );

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

	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( camera.GetMatrix() );

	// objects
	for ( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed( VK_F1 ) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}

	// imgui
	if ( wnd.Gfx().IsImGuiEnabled() )
	{
		if ( ImGui::Begin( "Main Debug Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize  ) )
		{
			if ( ImGui::CollapsingHeader( "Simulation Speed" ) )
			{
				ImGui::SliderFloat( "Speed Factor", &speed_factor, 0.0f, 4.0f );
				static char char_buffer[512];
				ImGui::InputText( "Hammerlock", char_buffer, sizeof( char_buffer ) );
			}

			if ( ImGui::CollapsingHeader( "Application Info" ) )
			{
				ImGui::Text( "Frametime: %.3f / Framerate: (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
			}

			if ( ImGui::CollapsingHeader( "About" ) )
			{
				ImGui::Text( "DirectX Project Demo by Kyle Robinson" );
				ImGui::NewLine();
				ImGui::Text( "Email: kylerobinson456@outlook.com" );
				ImGui::Text( "Twitter: @KyleRobinson42" );
			}
		}
		ImGui::End();

		camera.SpawnControlWindow();
	}
	
	wnd.Gfx().EndFrame();
}