#pragma once
#include "RenderQueuePass.h"
#include "Blender.h"
#include "NullPixelShader.h"
#include "Rasterizer.h"
#include "Stencil.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "RenderTarget.h"
#include "Source.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		ShadowMappingPass( Graphics& gfx, std::string name ) :
			RenderQueuePass( std::move( name ) )
		{
			depthStencil = std::make_unique<Bind::ShaderInputDepthStencil>( gfx, 3, Bind::DepthStencil::Usage::ShadowDepth );
			AddBind( Bind::VertexShader::Resolve( gfx, "SolidVS.cso" ) );
			AddBind( Bind::NullPixelShader::Resolve( gfx ) );
			AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off ) );
			AddBind( Bind::Blender::Resolve( gfx, false ) );
			RegisterSource( DirectBindableSource<Bind::DepthStencil>::Make( "shadowMap", depthStencil ) );
		}
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			depthStencil->Clear( gfx );
			pShadowCamera->BindToGraphics( gfx );
			RenderQueuePass::Execute( gfx );
		}
		void DumpShadowMap( Graphics& gfx, const std::string& path ) const
		{
			depthStencil->ToSurface( gfx ).Save( path );
		}
	private:
		const Camera* pShadowCamera = nullptr;
	};
}