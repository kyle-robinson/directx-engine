#include "ScaleOutlineRG.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawPass.h"
#include "OutlineMaskPass.h"

namespace Rgph
{
	ScaleOutlineRG::ScaleOutlineRG(Graphics& gfx) :
		RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clear");
			pass->SetSinkLinkage("renderTarget", "$.backbuffer");
			pass->SetSinkLinkage("depthStencil", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clear.renderTarget");
			pass->SetSinkLinkage("depthStencil", "clear.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskPass>(gfx, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineDrawPass>(gfx, "outlineDraw");
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "outlineDraw.renderTarget");
		Finalize();
	}
}