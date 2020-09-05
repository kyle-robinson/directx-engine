#pragma once
#include "RenderGraph.h"

class Graphics;

namespace Rgph
{
	class ScaleOutlineRG : public RenderGraph
	{
	public:
		ScaleOutlineRG(Graphics& gfx);
	};
}