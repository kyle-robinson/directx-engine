#include "NormalPlane.h"
#include "BindableCommon.h"
#include "Plane.h"
#include "TransformCbufDouble.h"
#include "imgui/imgui.h"

NormalPlane::NormalPlane( Graphics& gfx, float size )
{
	/*auto model = Plane::Make();
	model.Transform( DirectX::XMMatrixScaling( size, size, 1.0f ) );
	const auto geometryTag = "$plane." + std::to_string( size );
	AddBind( Bind::VertexBuffer::Resolve( gfx, geometryTag, model.vertices ) );
	AddBind( Bind::IndexBuffer::Resolve( gfx, geometryTag, model.indices ) );

	AddBind( std::make_unique<Bind::Texture>( gfx, "res\\textures\\brickwall.jpg" ) );
	AddBind( std::make_unique<Bind::Texture>( gfx, "res\\textures\\brickwall_normal_obj.png", 2u ) );

	auto pvs = Bind::VertexShader::Resolve( gfx, "PhongVS.cso" );
	auto pvsbc = pvs->GetByteCode();
	AddBind( std::move( pvs ) );
	AddBind( Bind::PixelShader::Resolve( gfx, "PhongPSNormalObject.cso" ) );

	AddBind( Bind::PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx, pmc, 1u ) );

	AddBind( Bind::InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
	AddBind( Bind::Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<Bind::TransformCbufDouble>( gfx, *this, 0u, 2u ) );*/
}

void NormalPlane::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void NormalPlane::SetRotation( float roll, float pitch, float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX NormalPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( roll, pitch, yaw ) *
		DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}

void NormalPlane::SpawnControlWindow( Graphics& gfx ) noexcept
{
	/*if ( ImGui::Begin( "Plane", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		if ( ImGui::CollapsingHeader( "Position" ) )
		{
			ImGui::SliderFloat( "X", &pos.x, -80.0f, 80.0f, "%.1f" );
			ImGui::SliderFloat( "Y", &pos.y, -80.0f, 80.0f, "%.1f" );
			ImGui::SliderFloat( "Z", &pos.z, -80.0f, 80.0f, "%.1f" );
		}

		if ( ImGui::CollapsingHeader( "Orientation" ) )
		{
			ImGui::SliderAngle( "Roll", &roll, -180.0f, 180.0f );
			ImGui::SliderAngle( "Pitch", &pitch, -180.0f, 180.0f );
			ImGui::SliderAngle( "Yaw", &yaw, -180.0f, 180.0f );
		}

		if ( ImGui::CollapsingHeader( "Shading" ) )
		{
			bool specInt = ImGui::SliderFloat( "Spec. Int", &pmc.specularIntensity, 0.0f, 1.0f );
			bool specPow = ImGui::SliderFloat( "Spec. Pow", &pmc.specularPower, 0.0f, 100.0f );
			bool checkState = pmc.normalMapEnabled == TRUE;
			bool normalEnabled = ImGui::Checkbox( "Normal Mapping", &checkState );
			pmc.normalMapEnabled = checkState ? TRUE : FALSE;
			
			if ( specInt || specPow || normalEnabled )
				QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx, pmc );
		}
	}
	ImGui::End();*/
}