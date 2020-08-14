#include "PointLight.h"
#include "res/imgui/imgui.h"

PointLight::PointLight( Graphics& gfx, float radius ) : mesh( gfx, radius ), cbuf( gfx )
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Light", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		//ImGui::PushItemWidth( 75 );

		if ( ImGui::CollapsingHeader( "Light" ) )
			ImGui::SliderFloat3( "Position", &cbData.lightPos.x, -30.0f, 30.0f, "%1.f" );

		if ( ImGui::CollapsingHeader( "Color" ) )
		{
			ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.0f, 2.0f, "%.2f");
			ImGui::ColorEdit3( "Diffuse", &cbData.diffuseColor.x );
			ImGui::ColorEdit3( "Material", &cbData.materialColor.x );
			ImGui::ColorEdit3( "Ambient", &cbData.ambient.x );

			ImGui::Text( "Attenuation" );
			ImGui::SliderFloat( "Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", 4 );
			ImGui::SliderFloat( "Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", 8 );
			ImGui::SliderFloat( "Quadratic", &cbData.attQuad, 0.0000001f, 1.0f, "%.7f", 10 );
		}
		
		if ( ImGui::Button( "Reset" ) )
			Reset();
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.7f, 0.7f, 0.9f },
		{ 0.05f, 0.05f, 0.05f },
		{ 1.0f, 1.0f, 1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f
	};
}

void PointLight::Draw( Graphics& gfx ) const noexcept(!IS_DEBUG)
{
	mesh.SetPos( cbData.lightPos );
	mesh.Draw( gfx );
}

void PointLight::Bind( Graphics& gfx ) const noexcept
{
	cbuf.Update( gfx, cbData );
	cbuf.Bind( gfx );
}