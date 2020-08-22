#include "NormalPlane.h"
#include "BindableCommon.h"
#include "Plane.h"
#include "imgui/imgui.h"

NormalPlane::NormalPlane( Graphics& gfx, float size )
{
	auto model = Plane::Make();
	model.Transform( DirectX::XMMatrixScaling( size, size, 1.0f ) );
	const auto geometryTag = "$plane." + std::to_string( size );
	AddBind( Bind::VertexBuffer::Resolve( gfx, geometryTag, model.vertices ) );
	AddBind( Bind::IndexBuffer::Resolve( gfx, geometryTag, model.indices ) );

	AddBind( Bind::Texture::Resolve( gfx, "res\\textures\\brickwall.jpg" ) );
	AddBind( Bind::Texture::Resolve( gfx, "res\\textures\\brickwall_normal.jpg" ) );

	auto pvs = Bind::VertexShader::Resolve( gfx, "PhongVS.cso" );
	auto pvsbc = pvs->GetByteCode();
	AddBind( std::move( pvs ) );
	AddBind( Bind::PixelShader::Resolve( gfx, "PhongPSNormal.cso" ) );

	AddBind( Bind::PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx, pmc, 1u ) );

	AddBind( Bind::InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
	AddBind( Bind::Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<Bind::TransformCbuf>( gfx, *this ) );
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