#pragma once
#include "RenderQueuePass.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"
#include "Sampler.h"
#include "Rasterizer.h"
#include "DepthStencil.h"
#include "Cube.h"
#include "CubeTexture.h"
#include "SkyboxTransformCbuf.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass( Graphics& gfx, std::string name ) :
			BindingPass( std::move( name ) )
		{
			RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( std::make_shared<Bind::CubeTexture>( gfx, "res\\skyboxes\\dark" ) );
			AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::DepthSkyBox ) );
			AddBind( Bind::Rasterizer::Resolve( gfx, true ) );
			AddBind( std::make_shared<Bind::SkyboxTransformCbuf>( gfx ) );
			AddBind( Bind::PixelShader::Resolve( gfx, "SkyboxPS.cso" ) );
			AddBind( Bind::Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
			{
				auto pvs = Bind::VertexShader::Resolve( gfx, "SkyboxVS.cso" );
				auto model = Cube::Make();
				const auto geometryTag = "$cube_map";
				AddBind( Bind::VertexBuffer::Resolve( gfx, geometryTag, std::move( model.vertices ) ) );
				count = static_cast<UINT>( model.indices.size() );
				AddBind( Bind::IndexBuffer::Resolve( gfx, geometryTag, std::move( model.indices ) ) );
				AddBind( Bind::InputLayout::Resolve( gfx, model.vertices.GetLayout(), *pvs ) );
				AddBind( std::move( pvs ) );
			}
			RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			assert( pMainCamera );
			pMainCamera->BindToGraphics( gfx );
			BindAll( gfx );
			gfx.DrawIndexed( count );
		}
	private:
		UINT count;
		const Camera* pMainCamera = nullptr;
	};
}