#pragma once
#include "RenderQueuePass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Rasterizer.h"
#include "Stencil.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class OutlineDrawPass : public RenderQueuePass
	{
	public:
		OutlineDrawPass(Graphics& gfx, std::string name) :
			RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::VertexShader::Resolve(gfx, "SolidVS.cso"));
			AddBind(Bind::PixelShader::Resolve(gfx, "SolidPS.cso"));
			AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Mask));
			AddBind(Bind::Rasterizer::Resolve(gfx, false));
		}
	};
}