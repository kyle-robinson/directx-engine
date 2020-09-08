#include "Camera.h"
#include "Math.h"
#include "Graphics.h"
#include "imgui/imgui.h"
#include <array>

Camera::Camera( Graphics& gfx, std::string name, DirectX::XMFLOAT3 initialPos,
	float initialPitch, float initialYaw, float initialTravSpeed, float initialRotSpeed ) noexcept
	:
	name( std::move( name ) ), initialPos( initialPos ), initialPitch( initialPitch ), initialYaw( initialYaw ),
	initialTravSpeed( initialTravSpeed ), initialRotSpeed( initialRotSpeed ),
	proj( gfx, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), indicator( gfx )
{
	std::array<Param, 3> type = { Param::Position, Param::Rotation, Param::Speed };
	for( auto& param : type ) Reset( param );

	indicator.SetPosition( pos );
	indicator.SetRotation( { pitch, yaw, 0.0f } );

	proj.SetPosition( pos );
	proj.SetRotation( { pitch, yaw, 0.0f } );
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

void Camera::SpawnControlWidgets( Graphics& gfx ) noexcept
{
	ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );

	if ( ImGui::CollapsingHeader( "Position" ) )
	{
		ImGui::SliderFloat( "X", &pos.x, -80.0f, 80.0f, "%.1f" );
		ImGui::SliderFloat( "Y", &pos.y, -80.0f, 80.0f, "%.1f" );
		ImGui::SliderFloat( "Z", &pos.z, -80.0f, 80.0f, "%.1f" );
		
		if ( ImGui::Button( "Reset Position" ) )
			Reset( Param::Position );
	}

	if ( ImGui::CollapsingHeader( "Orientation" ) )
	{
		ImGui::SliderAngle( "Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f );
		ImGui::SliderAngle( "Yaw", &yaw, -180.0f, 180.0f );

		if ( ImGui::Button( "Reset Rotation" ) )
			Reset( Param::Rotation );
	}

	if ( ImGui::CollapsingHeader( "Speed" ) )
	{
		ImGui::SliderFloat( "Movement", &travelSpeed, 1.0f, 20.0f, "%.1f" );
		ImGui::SliderFloat( "Sensitivity", &rotationSpeed, 0.0001f, 0.01f );

		if ( ImGui::Button( "Reset Speed" ) )
			Reset( Param::Speed );
	}

	proj.RenderWidgets( gfx );
}

void Camera::Reset( Param param ) noexcept
{
	switch ( param )
	{
	case Param::Position:
		pos = initialPos;
		return;
	case Param::Rotation:
		pitch = initialPitch;
		yaw = initialYaw;
		return;
	case Param::Speed:
		travelSpeed = initialTravSpeed;
		rotationSpeed = initialRotSpeed;
		return;
	default:
		assert( "Falied to reset camera parameters!" && false );
		return;
	}
}

void Camera::Rotate( float dx, float dy ) noexcept
{
	yaw = wrap_angle( yaw + dx * rotationSpeed );
	pitch = std::clamp( pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f );
	
	const DirectX::XMFLOAT3 angles = { pitch, yaw, 0.0f };
	indicator.SetRotation( angles );
	proj.SetRotation( angles );
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

	indicator.SetPosition( pos );
	proj.SetPosition( pos );
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}

void Camera::LinkTechniques( Rgph::RenderGraph& rg )
{
	indicator.LinkTechniques( rg );
	proj.LinkTechniques( rg );
}

void Camera::Submit() const
{
	indicator.Submit();
	proj.Submit();
}