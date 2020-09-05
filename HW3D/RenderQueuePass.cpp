#include "RenderQueuePass.h"


namespace Rgph
{
	void RenderQueuePass::Accept(Job job) noexcept
	{
		jobs.push_back(job);
	}

	void RenderQueuePass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		BindAll(gfx);

		for (const auto& j : jobs)
		{
			j.Execute(gfx);
		}
	}

	void RenderQueuePass::Reset() noexcept(!IS_DEBUG)
	{
		jobs.clear();
	}

}