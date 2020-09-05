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
			pass->SetInputSource("renderTarget", "$.backbuffer");
			pass->SetInputSource("depthStencil", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetInputSource("renderTarget", "clear.renderTarget");
			pass->SetInputSource("depthStencil", "clear.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskPass>(gfx, "outlineMask");
			pass->SetInputSource("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineDrawPass>(gfx, "outlineDraw");
			pass->SetInputSource("renderTarget", "lambertian.renderTarget");
			pass->SetInputSource("depthStencil", "outlineMask.depthStencil");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "outlineDraw.renderTarget");
		Finalize();
	}
}