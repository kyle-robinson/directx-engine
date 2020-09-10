#include "SolidSphere.h"
#include "Sphere.h"
#include "Vertex.h"
#include "Channels.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"

SolidSphere::SolidSphere( Graphics& gfx, float radius )
{
	using namespace Bind;

	auto model = Sphere::Make();
	model.Transform( DirectX::XMMatrixScaling( radius, radius, radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	pVertices = VertexBuffer::Resolve( gfx, geometryTag, model.vertices );
	pIndices = IndexBuffer::Resolve( gfx, geometryTag, model.indices );
	pTopology = Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique solid{ Channel::main };
		Step initial( "lambertian" );

		auto pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
		initial.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), *pvs ) );
		initial.AddBindable( std::move( pvs ) );
		initial.AddBindable( PixelShader::Resolve( gfx, "SolidPS.cso" ) );

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
			float padding;
		} colorConst;
		initial.AddBindable( PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 1u ) );
		initial.AddBindable( std::make_shared<TransformCbuf>( gfx ) );
		initial.AddBindable( Rasterizer::Resolve( gfx, false ) );

		solid.AddStep( std::move( initial ) );
		AddTechnique( std::move( solid ) );
	}
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}