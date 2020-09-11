#include "Camera.h"
#include "Math.h"
#include "Graphics.h"
#include "imgui/imgui.h"
#include <array>

Camera::Camera( Graphics& gfx, std::string name, DirectX::XMFLOAT3 initialPos,
	float initialPitch, float initialYaw, float initialTravSpeed, float initialRotSpeed, bool tethered ) noexcept
	:
	name( std::move( name ) ), initialPos( initialPos ), initialPitch( initialPitch ), initialYaw( initialYaw ),
	initialTravSpeed( initialTravSpeed ), initialRotSpeed( initialRotSpeed ), tethered( tethered ),
	proj( gfx, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ), indicator( gfx )
{
	if ( tethered )
	{
		pos = initialPos;
		indicator.SetPosition( pos );
		proj.SetPosition( pos );
	}
	
	std::array<Param, 3> type = { Param::Position, Param::Rotation, Param::Speed };
	for( auto& param : type ) Reset( gfx, param );
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

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return proj.GetMatrix();
}

void Camera::SpawnControlWidgets( Graphics& gfx ) noexcept
{
	bool posCheck = false;
	bool rotCheck = false;
	const auto linkCheck = []( bool changed, bool& carry ) { carry = carry || changed; };
	
	ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ) );
	
	if ( !tethered )
	{
		if ( ImGui::CollapsingHeader( "Position" ) )
		{
			linkCheck( ImGui::SliderFloat( "X", &pos.x, -80.0f, 80.0f, "%.1f" ), posCheck );
			linkCheck( ImGui::SliderFloat( "Y", &pos.y, -80.0f, 80.0f, "%.1f" ), posCheck );
			linkCheck( ImGui::SliderFloat( "Z", &pos.z, -80.0f, 80.0f, "%.1f" ), posCheck );
		
			if ( ImGui::Button( "Reset Position" ) )
				Reset( gfx, Param::Position );
		}
	}

	if ( ImGui::CollapsingHeader( "Orientation" ) )
	{
		linkCheck( ImGui::SliderAngle( "Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f ), rotCheck );
		linkCheck( ImGui::SliderAngle( "Yaw", &yaw, -180.0f, 180.0f ), rotCheck );

		if ( ImGui::Button( "Reset Rotation" ) )
			Reset( gfx, Param::Rotation );
	}

	if ( ImGui::CollapsingHeader( "Speed" ) )
	{
		if( !tethered )
			ImGui::SliderFloat( "Movement", &travelSpeed, 1.0f, 20.0f, "%.1f" );
		ImGui::SliderFloat( "Sensitivity", &rotationSpeed, 0.0001f, 0.01f );

		if ( ImGui::Button( "Reset Speed" ) )
			Reset( gfx, Param::Speed );
	}

	proj.RenderWidgets( gfx );

	if ( ImGui::CollapsingHeader( "Indicators" ) )
	{
		ImGui::Checkbox( "Camera Indicator", &enableCameraIndicator );
		ImGui::Checkbox( "Frustum Indicator", &enableFrustumIndicator );

		if ( ImGui::Button( "Reset Indicators" ) )
			Reset( gfx, Param::Indicator );
	}

	if ( posCheck )
	{
		indicator.SetPosition( pos );
		proj.SetPosition( pos );
	}

	if ( rotCheck )
	{
		const DirectX::XMFLOAT3 angles = { pitch, yaw, 0.0f };
		indicator.SetRotation( angles );
		proj.SetRotation( angles );
	}

	ImGui::PopStyleColor();
}

void Camera::Reset( Graphics& gfx, Param param ) noexcept
{
	pitch = initialPitch;
	yaw = initialYaw;
	const DirectX::XMFLOAT3 angles = { pitch, yaw, 0.0f };

	switch ( param )
	{
	case Param::Position:
		if ( !tethered )
		{
			pos = initialPos;
			indicator.SetPosition( pos );
			proj.SetPosition( pos );
		}
		proj.Reset( gfx );
		break;
	case Param::Rotation:
		indicator.SetRotation( angles );
		proj.SetRotation( angles );
		break;
	case Param::Speed:
		travelSpeed = initialTravSpeed;
		rotationSpeed = initialRotSpeed;
		break;
	case Param::Indicator:
		enableCameraIndicator = true;
		enableFrustumIndicator = true;
		break;
	default:
		throw std::exception( "ERROR:: An invalid camera param tried to be reset!" );
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
	if ( !tethered )
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
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return pos;
}

void Camera::SetPosition( const DirectX::XMFLOAT3& pos ) noexcept
{
	this->pos = pos;
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

void Camera::Submit( size_t channel ) const
{
	if ( enableCameraIndicator )
		indicator.Submit( channel );
	if ( enableFrustumIndicator )
		proj.Submit( channel );
}