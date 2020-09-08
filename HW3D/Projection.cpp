#include "Projection.h"
#include "Graphics.h"
#include "imgui/imgui.h"

Projection::Projection( Graphics& gfx, float initialWidth, float initialHeight, float initialNearZ, float initialFarZ )
	:
	initialWidth( initialWidth ), initialHeight( initialHeight ), initialNearZ( initialNearZ ), initialFarZ( initialFarZ ),
	frust( gfx, initialWidth, initialHeight, initialNearZ, initialFarZ )
{
	Reset();
}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH( width, height, nearZ, farZ );
}

void Projection::RenderWidgets( Graphics& gfx )
{
	if ( ImGui::CollapsingHeader( "Projection" ) )
	{
		bool bufferSet = false;
		const auto linkCheck = [&bufferSet]( bool changed ) { bufferSet = bufferSet || changed; };

		linkCheck( ImGui::SliderFloat( "Width", &width, 0.01f, 4.0f, "%.2f", 1.5f ) );
		linkCheck( ImGui::SliderFloat( "Height", &height, 0.01f, 4.0f, "%.2f", 1.5f ) );
		linkCheck( ImGui::SliderFloat( "Near Z", &nearZ, 0.01f, 400.0f, "%.2f", 4.0f ) );
		linkCheck( ImGui::SliderFloat( "Far Z", &farZ, 0.01f, 400.0f, "%.2f", 4.0f ) );

		if ( ImGui::Button( "Reset Projection" ) )
			Reset();

		if ( bufferSet )
			frust.SetVertices( gfx, width, height, nearZ, farZ );
	}
	ImGui::PopStyleColor();
}

void Projection::SetPosition( DirectX::XMFLOAT3 pos )
{
	frust.SetPosition( pos );
}

void Projection::SetRotation( DirectX::XMFLOAT3 rot )
{
	frust.SetRotation( rot );
}

void Projection::LinkTechniques( Rgph::RenderGraph& rg )
{
	frust.LinkTechniques( rg );
}

void Projection::Submit() const
{
	frust.Submit();
}

void Projection::Reset()
{
	width = initialWidth;
	height = initialHeight;
	nearZ = initialNearZ;
	farZ = initialFarZ;
}