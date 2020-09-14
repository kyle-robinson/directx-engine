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

	{
		Technique solid{ Channel::main };
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
		initial.AddBindable( std::make_shared<TransformCbuf>( gfx ) );
		initial.AddBindable( Rasterizer::Resolve( gfx, false ) );

		solid.AddStep( std::move( initial ) );
		AddTechnique( std::move( solid ) );
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

			return bufferSet;
		}
	} probe;
	Accept( probe );
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}