#include "VerticalBlurPass.h"
#include "Sink.h"
#include "Source.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"

namespace Rgph
{
	VerticalBlurPass::VerticalBlurPass( std::string name, Graphics& gfx ) :
		FullscreenPass( std::move( name ), gfx )
	{
		AddBind( Bind::PixelShader::Resolve( gfx, "BlurOutline_PS.cso" ) );
		AddBind( Bind::Blender::Resolve( gfx, true ) );
		AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Mask ) );

		RegisterSink( DirectBindableSink<Bind::Bindable>::Make( "scratchIn", blurScratchIn ) );
		RegisterSink( DirectBindableSink<Bind::Bindable>::Make( "control", control ) );
		RegisterSink( DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make( "direction", direction ) );
		RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );

		RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
	}

	void VerticalBlurPass::Execute( Graphics& gfx ) const noexcept(!IS_DEBUG)
	{
		auto buffer = direction->GetBuffer();
		buffer["isHorizontal"] = false;
		direction->SetBuffer( buffer );

		control->Bind( gfx );
		direction->Bind( gfx );
		blurScratchIn->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}