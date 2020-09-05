#pragma once
#include "RenderQueuePass.h"
#include "NullPixelShader.h"
#include "VertexShader.h"
#include "Rasterizer.h"
#include "Stencil.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class OutlineMaskPass : public RenderQueuePass
	{
	public:
		OutlineMaskPass(Graphics& gfx, std::string name) :
			RenderQueuePass(std::move(name))
		{
			RegisterInput(BufferInput<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterOutput(BufferOutput<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::VertexShader::Resolve(gfx, "SolidVS.cso"));
			AddBind(Bind::NullPixelShader::Resolve(gfx));
			AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Write));
			AddBind(Bind::Rasterizer::Resolve(gfx, false));
		}
	};
}