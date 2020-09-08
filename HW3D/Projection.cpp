#include "Projection.h"
#include "imgui/imgui.h"

Projection::Projection( float initialWidth, float initialHeight, float initialNearZ, float initialFarZ ) :
	initialWidth( initialWidth ), initialHeight( initialHeight ), initialNearZ( initialNearZ ), initialFarZ( initialFarZ )
{
	Reset();
}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH( width, height, nearZ, farZ );
}

void Projection::RenderWidgets()
{
	if ( ImGui::CollapsingHeader( "Projection" ) )
	{
		ImGui::SliderFloat( "Width", &width, 0.01f, 4.0f, "%.2f", 1.5f );
		ImGui::SliderFloat( "Height", &height, 0.01f, 4.0f, "%.2f", 1.5f );
		ImGui::SliderFloat( "Near Z", &nearZ, 0.01f, 400.0f, "%.2f", 4.0f );
		ImGui::SliderFloat( "Far Z", &farZ, 0.01f, 400.0f, "%.2f", 4.0f );

		if ( ImGui::Button( "Reset Projection" ) )
			Reset();
	}
	ImGui::PopStyleColor();
}

void Projection::Reset()
{
	width = initialWidth;
	height = initialHeight;
	nearZ = initialNearZ;
	farZ = initialFarZ;
}