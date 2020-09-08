#include "Camera.h"
#include "Math.h"
#include "Graphics.h"
#include "imgui/imgui.h"

Camera::Camera( std::string name, DirectX::XMFLOAT3 initialPos, float initialPitch, float initialYaw ) noexcept :
	name( std::move( name ) ), initialPos( initialPos ), initialPitch( initialPitch ), initialYaw( initialYaw ),
	proj( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f )
{
	Reset();
}

void Camera::BindToGraphics( Graphics& gfx ) const
{
	gfx.SetCamera( GetMatrix() );
	gfx.SetProjection( proj.GetMatrix() );
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// facing direction
	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );

	// apply camera rotations to base vector
	const auto lookVector = DirectX::XMVector3Transform(
		forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f )
	);

	// generate camera position
	const auto camPosition = DirectX::XMLoadFloat3( &pos );
	const auto camTarget = DirectX::XMVectorAdd( camPosition, lookVector );
	return DirectX::XMMatrixLookAtLH( camPosition, camTarget, DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}

void Camera::SpawnControlWidgets() noexcept
{
	if ( ImGui::CollapsingHeader( "Position" ) )
	{
		ImGui::SliderFloat( "X", &pos.x, -80.0f, 80.0f, "%.1f" );
		ImGui::SliderFloat( "Y", &pos.y, -80.0f, 80.0f, "%.1f" );
		ImGui::SliderFloat( "Z", &pos.z, -80.0f, 80.0f, "%.1f" );
	}

	if ( ImGui::CollapsingHeader( "Orientation" ) )
	{
		ImGui::SliderAngle( "Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f );
		ImGui::SliderAngle( "Yaw", &yaw, -180.0f, 180.0f );
	}

	if ( ImGui::CollapsingHeader( "Speed" ) )
	{
		ImGui::SliderFloat( "Movement", &travelSpeed, 1.0f, 20.0f, "%.1f" );
		ImGui::SliderFloat( "Rotation", &rotationSpeed, 0.0001f, 0.01f );
	}
		
	if ( ImGui::Button( "Reset" ) )
		Reset();

	proj.RenderWidgets();
}

void Camera::Reset() noexcept
{
	pos = initialPos;
	pitch = initialPitch;
	yaw = initialYaw;
}

void Camera::Rotate( float dx, float dy ) noexcept
{
	yaw = wrap_angle( yaw + dx * rotationSpeed );
	pitch = std::clamp( pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f );
}

void Camera::Translate( DirectX::XMFLOAT3 translation ) noexcept
{
	DirectX::XMStoreFloat3(
		&translation,
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3( &translation ),
			DirectX::XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f ) *
			DirectX::XMMatrixScaling( travelSpeed, travelSpeed, travelSpeed )
		)
	);

	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}