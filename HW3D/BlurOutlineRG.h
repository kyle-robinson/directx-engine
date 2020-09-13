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
		void RenderWindows( Graphics& gfx, bool loadShadow, bool loadBlur );
		void DumpShadowMap( Graphics& gfx, const std::string& path );
		void BindMainCamera( Camera& cam );
		void BindShadowCamera( Camera& cam );
	private:
		void RenderKernelWindow( Graphics& gfx );
		//void RenderShadowWindow( Graphics& gfx );
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
		//std::shared_ptr<Bind::CachingPixelConstantBufferEx> shadowControl;
		//std::shared_ptr<Bind::ShadowSampler> shadowSampler;
		//std::shared_ptr<Bind::ShadowRasterizer> shadowRasterizer;
	};
}