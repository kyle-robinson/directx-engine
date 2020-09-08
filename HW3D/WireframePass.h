#pragma once
#include "RenderQueuePass.h"
#include "Stencil.h"
#include "Source.h"
#include "Sink.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class WireframePass : public RenderQueuePass
	{
	public:
		WireframePass( Graphics& gfx, std::string name ) :
			RenderQueuePass( std::move( name ) )
		{
			RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget", renderTarget) );
			RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::DepthReversed ) );
		}
	};
}