#include "VerticalBlurPass.h"
#include "PassInput.h"
#include "PassOutput.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"

VerticalBlurPass::VerticalBlurPass( std::string name, Graphics& gfx ) :
	FullscreenPass( std::move( name ), gfx )
{
	AddBind( Bind::PixelShader::Resolve( gfx, "BlurOutline_PS.cso" ) );
	AddBind( Bind::Blender::Resolve( gfx, true ) );
	AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Mask ) );

	RegisterInput( ImmutableInput<Bind::Bindable>::Make( "scratchIn", blurScratchIn ) );
	RegisterInput( ImmutableInput<Bind::Bindable>::Make( "control", control ) );
	RegisterInput( ImmutableInput<Bind::CachingPixelConstantBufferEx>::Make( "direction", direction ) );
	RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
	RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );

	RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
	RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
}

void VerticalBlurPass::Execute( Graphics& gfx ) const noexcept
{
	auto buffer = direction->GetBuffer();
	buffer["isHorizontal"] = false;
	direction->SetBuffer( buffer );

	control->Bind( gfx );
	direction->Bind( gfx );
	blurScratchIn->Bind( gfx );
	FullscreenPass::Execute( gfx );
}