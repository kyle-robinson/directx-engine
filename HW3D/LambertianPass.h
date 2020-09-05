#pragma once
#include "RenderQueuePass.h"
#include "PassOutput.h"
#include "PassInput.h"
#include "Stencil.h"
#include "Job.h"
#include <vector>

class Graphics;

class LambertianPass : public RenderQueuePass
{
public:
	LambertianPass( Graphics& gfx, std::string name ) : RenderQueuePass( std::move( name ) )
	{
		RegisterInput(BufferInput<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterInput(BufferInput<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		RegisterOutput(BufferOutput<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterOutput(BufferOutput<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		AddBind( Bind::Stencil::Resolve( gfx, Bind::Stencil::Mode::Off ) );
	}
};