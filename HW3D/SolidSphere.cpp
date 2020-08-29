#include "SolidSphere.h"
#include "Sphere.h"
#include "Vertex.h"
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
		Technique solid;
		Step initial( 0 );

		auto pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		initial.AddBindable( std::move( pvs ) );
		initial.AddBindable( PixelShader::Resolve( gfx, "SolidPS.cso" ) );

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
			float padding;
		} colorConst;
		initial.AddBindable( PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 1u ) );

		initial.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );
		initial.AddBindable( std::make_shared<TransformCbuf>( gfx ) );
		initial.AddBindable( Blender::Resolve( gfx, false ) );
		initial.AddBindable( Rasterizer::Resolve( gfx, false ) );

		solid.AddStep( std::move( initial ) );
		AddTechnique( std::move( solid ) );
	}

	/*AddBind( VertexBuffer::Resolve( gfx, geometryTag, model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx, geometryTag, model.indices ) );

	auto pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
	auto pvsbc = pvs->GetByteCode();
	AddBind( std::move( pvs ) );
	AddBind( PixelShader::Resolve( gfx, "SolidPS.cso" ) );

	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} colorConst;
	AddBind( PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 1u ) );

	AddBind( InputLayout::Resolve( gfx, model.vertices.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gfx, *this ) );

	AddBind( std::make_shared<Stencil>( gfx, Stencil::Mode::Off ) );*/
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}