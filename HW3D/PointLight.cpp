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
		ImGui::PushItemWidth(75);

		if ( ImGui::CollapsingHeader( "Position" ) )
		{
			ImGui::SliderFloat( "X", &cbData.lightPos.x, -30.0f, 30.0f, "%1.f" ); ImGui::SameLine();
			ImGui::SliderFloat( "Y", &cbData.lightPos.y, -30.0f, 30.0f, "%1.f" ); ImGui::SameLine();
			ImGui::SliderFloat( "Z", &cbData.lightPos.z, -30.0f, 30.0f, "%1.f" );
		}

		if ( ImGui::CollapsingHeader( "Color" ) )
		{
			ImGui::Text( "Diffuse" );
			ImGui::SliderFloat( "R###Diffuse", &cbData.diffuseColor.x, 0.0f, 1.0f, "%.1f" ); ImGui::SameLine();
			ImGui::SliderFloat( "G###Diffuse", &cbData.diffuseColor.y, 0.0f, 1.0f, "%.1f" ); ImGui::SameLine();
			ImGui::SliderFloat( "B###Diffuse", &cbData.diffuseColor.z, 0.0f, 1.0f, "%.1f" );

			ImGui::Text( "Material" );
			ImGui::SliderFloat( "R###Material", &cbData.materialColor.x, 0.0f, 1.0f, "%.1f" ); ImGui::SameLine();
			ImGui::SliderFloat( "G###Material", &cbData.materialColor.y, 0.0f, 1.0f, "%.1f" ); ImGui::SameLine();
			ImGui::SliderFloat( "B###Material", &cbData.materialColor.z, 0.0f, 1.0f, "%.1f" );

			ImGui::Text("Ambient");
			ImGui::SliderFloat("R###Ambient", &cbData.ambient.x, 0.0f, 1.0f, "%.1f"); ImGui::SameLine();
			ImGui::SliderFloat("G###Ambient", &cbData.ambient.y, 0.0f, 1.0f, "%.1f"); ImGui::SameLine();
			ImGui::SliderFloat("B###Ambient", &cbData.ambient.z, 0.0f, 1.0f, "%.1f");

			ImGui::NewLine();

			ImGui::SliderFloat("Diffuse Intensity", &cbData.diffuseIntensity, 0.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Att. Const", &cbData.attConst, 0.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Att. Linear", &cbData.attLin, 0.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Att. Quadratic", &cbData.attQuad, 0.0f, 1.0f, "%.1f");
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