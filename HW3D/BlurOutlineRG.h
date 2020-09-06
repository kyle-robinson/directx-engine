#pragma once
#include "RenderGraph.h"
#include "ConstantBufferEx.h"
#include <memory>

class Graphics;

namespace Bind
{
	class Bindable;
	class RenderTarget;
}

namespace Rgph
{
	class BlurOutlineRG : public RenderGraph
	{
	public:
		BlurOutlineRG(Graphics& gfx);
	private:
		void SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG);
		static constexpr int maxRadius = 7;
		static constexpr int radius = 4;
		static constexpr float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}