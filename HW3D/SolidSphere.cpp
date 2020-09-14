#include "SolidSphere.h"
#include "Sphere.h"
#include "Vertex.h"
#include "Channels.h"
#include "DynamicConstant.h"
#include "ConstantBufferEx.h"
#include "TechniqueProbe.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "imgui/imgui.h"

SolidSphere::SolidSphere( Graphics& gfx, float radius )
{
	using namespace Bind;

	auto model = Sphere::Make();
	model.Transform( DirectX::XMMatrixScaling( radius, radius, radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	pVertices = VertexBuffer::Resolve( gfx, geometryTag, model.vertices );
	pIndices = IndexBuffer::Resolve( gfx, geometryTag, model.indices );
	pTopology = Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	auto tcb = std::make_shared<TransformCbuf>( gfx );
	{
		Technique solid( "Shading", Channel::main );
		{
			Step initial( "lambertian" );

			auto pvs = VertexShader::Resolve( gfx, "SolidVS.cso" );
			initial.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), *pvs ) );
			initial.AddBindable( std::move( pvs ) );
			initial.AddBindable( PixelShader::Resolve( gfx, "SolidPS.cso" ) );

			Dcb::RawLayout layout;
			layout.Add<Dcb::Float3>( "color" );
			auto buffer = Dcb::Buffer( std::move( layout ) );
			buffer["color"] = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };

			initial.AddBindable( std::make_shared<CachingPixelConstantBufferEx>( gfx, buffer, 1u ) );
			initial.AddBindable( tcb );
			initial.AddBindable( Rasterizer::Resolve( gfx, false ) );

			solid.AddStep( std::move( initial ) );
		}
		AddTechnique( std::move( solid ) );
	}

	{
		Technique outline( "Outline", Channel::main );
		{
			Step mask( "outlineMask" );

			mask.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), *VertexShader::Resolve( gfx, "SolidVS.cso" ) ) );
			mask.AddBindable( std::move( tcb ) );

			outline.AddStep( std::move( mask ) );
		}
		{
			Step draw( "outlineDraw" );

			Dcb::RawLayout layout;
			layout.Add<Dcb::Float4>( "blurColor" );
			auto buf = Dcb::Buffer( std::move( layout ) );
			buf["blurColor"] = DirectX::XMFLOAT4{ 1.0f, 0.4f, 0.4f, 1.0f };
			draw.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx, buf, 1u ) );

			draw.AddBindable( InputLayout::Resolve( gfx, model.vertices.GetLayout(), *VertexShader::Resolve( gfx, "SolidVS.cso" ) ) );
			draw.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			outline.AddStep( std::move( draw ) );
		}
		AddTechnique( std::move( outline ) );
	}
}

void SolidSphere::SetPosition( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void SolidSphere::SpawnControls() noexcept
{
	class Probe : public TechniqueProbe
	{
	public:
		bool OnVisitBuffer( Dcb::Buffer& buf ) override
		{
			float bufferSet = false;
			const auto linkCheck = [&bufferSet]( bool changed ) { bufferSet = bufferSet || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string( bufIdx )]
			( const char* label ) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if ( auto v = buf["color"]; v.Exists() )
				linkCheck( ImGui::ColorEdit3( tag( "Sphere" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT3&>( v ) ) ) );
			if ( auto v = buf["blurColor"]; v.Exists() )
				linkCheck( ImGui::ColorEdit4( tag( "Outline" ), reinterpret_cast<float*>( &static_cast<DirectX::XMFLOAT4&>( v ) ) ) );

			return bufferSet;
		}
	} probe;
	Accept( probe );
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}