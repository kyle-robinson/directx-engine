#include "PointLight.h"
#include "res/imgui/imgui.h"

PointLight::PointLight( Graphics& gfx, float radius ) : mesh( gfx, radius ), cbuf( gfx ) { }

void PointLight::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Light", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Position" ) )
		{
			ImGui::SliderFloat( "X", &pos.x, -60.0f, 60.0f, "%.1f" );
			ImGui::SliderFloat( "Y", &pos.y, -60.0f, 60.0f, "%.1f" );
			ImGui::SliderFloat( "Z", &pos.z, -60.0f, 60.0f, "%.1f" );
		}

		if ( ImGui::CollapsingHeader( "Color" ) )
		{
			ImGui::SliderFloat( "R", &color.x, 0.0f, 1.0f, "%.1f" );
			ImGui::SliderFloat( "G", &color.y, 0.0f, 1.0f, "%.1f" );
			ImGui::SliderFloat( "B", &color.z, 0.0f, 1.0f, "%.1f" );
		}
		
		if ( ImGui::Button( "Reset" ) )
			Reset();
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	pos = { 0.0f, 0.0f, 0.0f };
	color = { 1.0f, 0.0f, 0.5f };
}

void PointLight::Draw( Graphics& gfx ) const noexcept(!IS_DEBUG)
{
	mesh.SetPos( pos );
	mesh.Draw( gfx );
}

void PointLight::Bind( Graphics& gfx ) const noexcept
{
	cbuf.Update( gfx, PointLightCBuf{ { pos }, { color } } );
	cbuf.Bind( gfx );
}