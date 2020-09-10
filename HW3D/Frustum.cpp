#include "Frustum.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"
#include "Channels.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"

Frustum::Frustum( Graphics& gfx, float width, float height, float nearZ, float farZ )
{
	std::vector<unsigned short> indices;
	{
		indices.push_back( 0 );
		indices.push_back( 1 );
		indices.push_back( 1 );
		indices.push_back( 2 );
		indices.push_back( 2 );
		indices.push_back( 3 );
		indices.push_back( 3 );
		indices.push_back( 0 );
		indices.push_back( 4 );
		indices.push_back( 5 );
		indices.push_back( 5 );
		indices.push_back( 6 );
		indices.push_back( 6 );
		indices.push_back( 7 );
		indices.push_back( 7 );
		indices.push_back( 4 );
		indices.push_back( 0 );
		indices.push_back( 4 );
		indices.push_back( 1 );
		indices.push_back( 5 );
		indices.push_back( 2 );
		indices.push_back( 6 );
		indices.push_back( 3 );
		indices.push_back( 7 );
	}

	SetVertices( gfx, width, height, nearZ, farZ );
	pIndices = Bind::IndexBuffer::Resolve( gfx, "$frustum", indices );
	pTopology = Bind::Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	{
		Technique line{ Channel::main };
		{
			Step unoccluded( "lambertian" );

			auto pvs = Bind::VertexShader::Resolve( gfx, "SolidVS.cso" );
			unoccluded.AddBindable( Bind::InputLayout::Resolve( gfx, pVertices->GetLayout(), *pvs ) );
			unoccluded.AddBindable( std::move( pvs ) );
			unoccluded.AddBindable( Bind::PixelShader::Resolve( gfx, "SolidPS.cso" ) );

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 0.6f, 0.2f, 0.2f };
				float padding;
			} colorConst;
			unoccluded.AddBindable( Bind::PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 1u ) );
			unoccluded.AddBindable( std::make_shared<Bind::TransformCbuf>( gfx ) );
			unoccluded.AddBindable( Bind::Rasterizer::Resolve( gfx, false ) );

			line.AddStep( std::move( unoccluded ) );
		}
		{
			Step occluded( "wireframe" );

			auto pvs = Bind::VertexShader::Resolve( gfx, "SolidVS.cso" );
			occluded.AddBindable( Bind::InputLayout::Resolve( gfx, pVertices->GetLayout(), *pvs ) );
			occluded.AddBindable( std::move( pvs ) );
			occluded.AddBindable( Bind::PixelShader::Resolve( gfx, "SolidPS.cso" ) );

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 0.25f, 0.08f, 0.08f };
				float padding;
			} colorConst;
			occluded.AddBindable( Bind::PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 1u ) );
			occluded.AddBindable( std::make_shared<Bind::TransformCbuf>( gfx ) );
			occluded.AddBindable( Bind::Rasterizer::Resolve( gfx, false ) );

			line.AddStep( std::move( occluded ) );
		}
		AddTechnique( std::move( line ) );
	}
}

void Frustum::SetVertices( Graphics& gfx, float width, float height, float nearZ, float farZ )
{
	VertexMeta::VertexLayout layout;
	layout.Append( VertexMeta::VertexLayout::Position3D );
	VertexMeta::VertexBuffer vertices{ std::move( layout ) };
	{
		const float zRatio = farZ / nearZ;
		const float nearX = width / 2.0f;
		const float nearY = height / 2.0f;
		const float farX = nearX * zRatio;
		const float farY = nearY * zRatio;
		vertices.EmplaceBack( DirectX::XMFLOAT3{ -nearX,nearY,nearZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ nearX,nearY,nearZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ nearX,-nearY,nearZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ -nearX,-nearY,nearZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ -farX,farY,farZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ farX,farY,farZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ farX,-farY,farZ } );
		vertices.EmplaceBack( DirectX::XMFLOAT3{ -farX,-farY,farZ } );
	}
	pVertices = std::make_shared<Bind::VertexBuffer>( gfx, vertices );
}

void Frustum::SetPosition( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void Frustum::SetRotation( DirectX::XMFLOAT3 rot ) noexcept
{
	this->rot = rot;
}

DirectX::XMMATRIX Frustum::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYawFromVector( DirectX::XMLoadFloat3( &rot ) ) *
		DirectX::XMMatrixTranslationFromVector( DirectX::XMLoadFloat3( &pos ) );
}