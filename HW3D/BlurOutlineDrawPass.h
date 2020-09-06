#pragma once
#include "RenderQueuePass.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Blender.h"
#include "Job.h"
#include <vector>

class Graphics;

namespace Rgph
{
	class BlurOutlineDrawPass : public RenderQueuePass
	{
	public:
		BlurOutlineDrawPass(Graphics& gfx, std::string name, unsigned int width, unsigned int height) :
			RenderQueuePass(std::move(name))
		{
			renderTarget = std::make_unique<Bind::ShaderInputRenderTarget>(gfx, width / 2, height / 2, 0);
			AddBind(Bind::VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(Bind::PixelShader::Resolve(gfx, "Solid_PS.cso"));
			AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Mask));
			AddBind(Bind::Blender::Resolve(gfx, false));
			RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", renderTarget));
		}
		void Execute( Graphics& gfx ) const noexcept(!IS_DEBUG) override
		{
			renderTarget->Clear( gfx );
			RenderQueuePass::Execute( gfx );
		}
	};
}