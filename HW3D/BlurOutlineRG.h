#pragma once
#include "RenderGraph.h"
#include "ConstantBufferEx.h"
#include <memory>

class Camera;
class Graphics;

namespace Bind
{
	class Bindable;
	class RenderTarget;
	class ShadowSampler;
	class ShadowRasterizer;
}

namespace Rgph
{
	class BlurOutlineRG : public RenderGraph
	{
	public:
		BlurOutlineRG( Graphics& gfx );
		void RenderKernelWindow( Graphics& gfx );
		void BindMainCamera( Camera& cam );
		void BindShadowCamera( Camera& cam );
	private:
		void SetKernelGauss( int radius, float sigma ) noexcept(!IS_DEBUG);
		void SetKernelBox( int radius ) noexcept(!IS_DEBUG);
		enum class KernelType
		{
			Gauss,
			Box
		} kernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}